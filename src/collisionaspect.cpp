/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of DrawSpace.
*
*    DrawSpace is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    DrawSpace is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include "collisionaspect.h"
#include "physicsaspect.h"
#include "transformaspect.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;

CollisionAspect::CollisionAspect(void)
{

}

CollisionAspect::~CollisionAspect(void)
{

}


btRigidBody* CollisionAspect::Init(void)
{
    if (m_initialized)
    {
        _DSEXCEPTION("collisionaspect already initialized")
    }

    Matrix attitude_mat;
    attitude_mat.Identity();

    btScalar    btmat[16];
    btTransform bt_transform;
    
    convert_matrix_to_bt(attitude_mat, btmat);
    bt_transform.setFromOpenGLMatrix(btmat);


    ComponentList<BoxCollisionShape> boxcollision_shapes;
    ComponentList<SphereCollisionShape> spherecollision_shapes;
    ComponentList<MesheCollisionShape> meshecollision_shapes;
    ComponentList<CompoundCollisionShape> compoundcollision_shapes;

    GetComponentsByType<BoxCollisionShape>(boxcollision_shapes);
    GetComponentsByType<SphereCollisionShape>(spherecollision_shapes);
    GetComponentsByType<MesheCollisionShape>(meshecollision_shapes);
    GetComponentsByType<CompoundCollisionShape>(compoundcollision_shapes);


    for (auto& e : boxcollision_shapes)
    {
        DrawSpace::Utils::Vector box_dims;
        box_dims = e->getPurpose().GetPos();

        btBoxShape* shape = _DRAWSPACE_NEW_(btBoxShape, btBoxShape(btVector3(box_dims[0], box_dims[1], box_dims[2])));

        Utils::Matrix transf = e->getPurpose().GetTransform();

        m_collisionShapesList.push_back(std::make_pair(shape, transf));
    }

    for (auto& e : spherecollision_shapes)
    {
        dsreal sphere_radius = spherecollision_shapes[0]->getPurpose().GetRay();;
        btSphereShape* shape = _DRAWSPACE_NEW_(btSphereShape, btSphereShape(sphere_radius));

        Utils::Matrix transf = e->getPurpose().GetTransform();

        m_collisionShapesList.push_back(std::make_pair(shape, transf));
    }

    for (auto& e : meshecollision_shapes)
    {
        Meshe meshe = meshecollision_shapes[0]->getPurpose().m_meshe;

        auto mesh{ _DRAWSPACE_NEW_(btTriangleMesh, btTriangleMesh) };

        for (long i = 0; i < meshe.GetTrianglesListSize(); i++)
        {
            Triangle curr_triangle;
            meshe.GetTriangles(i, curr_triangle);

            Vertex v1, v2, v3;

            meshe.GetVertex(curr_triangle.vertex1, v1);
            meshe.GetVertex(curr_triangle.vertex2, v2);
            meshe.GetVertex(curr_triangle.vertex3, v3);

            btVector3 a(v1.x, v1.y, v1.z);
            btVector3 b(v2.x, v2.y, v2.z);
            btVector3 c(v3.x, v3.y, v3.z);

            mesh->addTriangle(a, b, c, false);
        }

        btBvhTriangleMeshShape* shape = _DRAWSPACE_NEW_(btBvhTriangleMeshShape, btBvhTriangleMeshShape(mesh, true, true));

        Utils::Matrix transf = e->getPurpose().GetTransform();

        m_collisionShapesList.push_back(std::make_pair(shape, transf));

        m_trianglesMeshes.push_back(mesh);
    }

    if (compoundcollision_shapes.size())
    {
        btCompoundShape* shape = _DRAWSPACE_NEW_(btCompoundShape, btCompoundShape);

        for (auto& e : m_collisionShapesList)
        {
            btScalar    btmat[16];
            btCollisionShape* sub_shape = e.first;
            Matrix mat = e.second;

            convert_matrix_to_bt(mat, btmat);

            btTransform bttransf;
            bttransf.setFromOpenGLMatrix(btmat);

            shape->addChildShape(bttransf, sub_shape);
        }
        m_compoundShape = shape;
        m_collisionShape = m_compoundShape;
    }
    else
    {
        // no compound declared...
        // get the last one only
        if (m_collisionShapesList.size())
        {
            m_collisionShape = m_collisionShapesList.back().first;
        }
    }

    ///////////////////////////////////////////////////////////////////////////

    m_motionState = _DRAWSPACE_NEW_(btDefaultMotionState, btDefaultMotionState(bt_transform));

    // collider only here, no mass, no movements :)
    btVector3 localInertia(0, 0, 0);
    btRigidBody::btRigidBodyConstructionInfo boxRigidBodyConstructionInfo(0.0, m_motionState, m_collisionShape, localInertia);

    m_rigidBody = _DRAWSPACE_NEW_(btRigidBody, btRigidBody(boxRigidBodyConstructionInfo));

    m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

    m_rigidBody->setActivationState(DISABLE_DEACTIVATION);

    m_initialized = true;
    return m_rigidBody;
}

btRigidBody* CollisionAspect::GetRigidBody(void) const
{
    return m_rigidBody;
}

void CollisionAspect::Release(void)
{
    if (!m_initialized)
    {
        _DSEXCEPTION("collisionaspect not initialized !")
    }

    _DRAWSPACE_DELETE_(m_motionState);

    for (auto& e : m_collisionShapesList)
    {
        _DRAWSPACE_DELETE_(e.first);
    }
    if (m_compoundShape)
    {
        _DRAWSPACE_DELETE_(m_compoundShape);
    }
    m_collisionShapesList.clear();

    if (m_world)
    {
        m_world->removeRigidBody(m_rigidBody);

        m_world = NULL;
        m_physical_aspect_owner = NULL;
    }

    _DRAWSPACE_DELETE_(m_rigidBody);
    m_rigidBody = nullptr;

    for (auto& e : m_trianglesMeshes)
    {
        _DRAWSPACE_DELETE_(e);
    }

    m_initialized = false;
}

void CollisionAspect::Update(Entity* p_owner_entity)
{
    if (m_initialized)
    {
        // read entity world transform, to set it into our physics collider
        TransformAspect* transform_aspect{ p_owner_entity->GetAspect<TransformAspect>() };
        if (transform_aspect)
        {
            Matrix world;

            transform_aspect->GetWorldTransformFromPhysicWorld(world);
          
            btScalar    btmat[16];
            btTransform bt_transform;

            convert_matrix_to_bt(world, btmat);
            bt_transform.setFromOpenGLMatrix(btmat);
            m_motionState->m_graphicsWorldTrans = bt_transform;
        }
    }
}

void CollisionAspect::RegisterPhysicalAspect(PhysicsAspect* p_physical_aspect)
{
    if (!m_initialized)
    {
        _DSEXCEPTION("collisionaspect not initialized")
    }

    if (NULL == m_physical_aspect_owner)
    {
        m_physical_aspect_owner = p_physical_aspect;
        m_world = m_physical_aspect_owner->GetWorld();
        m_world->addRigidBody(m_rigidBody);
    }
    else
    {
        // ce body passe d'un world a l'autre (attachment/detachment)

        m_world->removeRigidBody(m_rigidBody);

        m_physical_aspect_owner = p_physical_aspect;
        m_world = m_physical_aspect_owner->GetWorld();
        m_world->addRigidBody(m_rigidBody);
    }
}

void CollisionAspect::convert_matrix_to_bt(const Matrix& p_mat, btScalar* bt_matrix)
{
    bt_matrix[0] = p_mat(0, 0);
    bt_matrix[1] = p_mat(0, 1);
    bt_matrix[2] = p_mat(0, 2);
    bt_matrix[3] = p_mat(0, 3);

    bt_matrix[4] = p_mat(1, 0);
    bt_matrix[5] = p_mat(1, 1);
    bt_matrix[6] = p_mat(1, 2);
    bt_matrix[7] = p_mat(1, 3);

    bt_matrix[8] = p_mat(2, 0);
    bt_matrix[9] = p_mat(2, 1);
    bt_matrix[10] = p_mat(2, 2);
    bt_matrix[11] = p_mat(2, 3);

    bt_matrix[12] = p_mat(3, 0);
    bt_matrix[13] = p_mat(3, 1);
    bt_matrix[14] = p_mat(3, 2);
    bt_matrix[15] = p_mat(3, 3);
}

void CollisionAspect::convert_matrix_from_bt(btScalar* bt_matrix, Utils::Matrix& p_mat)
{
    p_mat(0, 0) = bt_matrix[0];
    p_mat(0, 1) = bt_matrix[1];
    p_mat(0, 2) = bt_matrix[2];
    p_mat(0, 3) = bt_matrix[3];

    p_mat(1, 0) = bt_matrix[4];
    p_mat(1, 1) = bt_matrix[5];
    p_mat(1, 2) = bt_matrix[6];
    p_mat(1, 3) = bt_matrix[7];

    p_mat(2, 0) = bt_matrix[8];
    p_mat(2, 1) = bt_matrix[9];
    p_mat(2, 2) = bt_matrix[10];
    p_mat(2, 3) = bt_matrix[11];

    p_mat(3, 0) = bt_matrix[12];
    p_mat(3, 1) = bt_matrix[13];
    p_mat(3, 2) = bt_matrix[14];
    p_mat(3, 3) = bt_matrix[15];
}
