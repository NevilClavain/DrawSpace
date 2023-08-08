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

#include "ds_types.h"
#include "physicsaspect.h"
#include "transformaspect.h"
#include "matrix.h"

#include "rigidbodytransformaspectimpl.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Maths;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;


RigidBodyTransformAspectImpl::RigidBodyTransformAspectImpl(void)
{
    m_stack_matrix_inv.identity();
    m_memorized_vectors = false;

    m_last_local_transf.identity();
}

void RigidBodyTransformAspectImpl::GetLocaleTransform(TransformAspect* p_transformaspect, Maths::Matrix& p_out_base_transform)
{
    if (m_initialized)
    {
        btScalar                 bt_matrix[16];
        DrawSpace::Maths::Matrix local_transf;

        m_motionState->m_graphicsWorldTrans.getOpenGLMatrix(bt_matrix);
        convert_matrix_from_bt(bt_matrix, local_transf);

        ComponentList<bool> flags;
        p_transformaspect->GetComponentsByType<bool>(flags);

        if (flags.size() >= 3)
        {
            bool& stop_linear_speed = flags[1]->getPurpose();
            bool& stop_angular_speed = flags[2]->getPurpose();

            if (stop_linear_speed)
            {
                m_rigidBody->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
                stop_linear_speed = false;
            }

            if (stop_angular_speed)
            {
                m_rigidBody->setAngularVelocity(btVector3(0.0, 0.0, 0.0));
                stop_angular_speed = false;
            }
        }

        ///////////////////////////////////////////////
        // application des forces & torques

        ComponentList<Force> forces;
        p_transformaspect->GetComponentsByType<Force>(forces);

        for (size_t i = 0; i < forces.size(); ++i)
        {
            Force applied_force = forces[i]->getPurpose();

            if (applied_force.m_enabled)
            {
                Vector force_v = applied_force.m_force_dir;
                force_v.scale(applied_force.m_force_scale);

                Vector final_force_v;

                if (Force::LOCALE == applied_force.m_mode)
                {
                    Matrix local_rot = local_transf;
                    local_rot.clearTranslation();
                    local_rot.transform(&force_v, &final_force_v);
                }
                else
                {
                    final_force_v = force_v;
                }

                m_rigidBody->applyForce(btVector3(final_force_v[0], final_force_v[1], final_force_v[2]),
                    btVector3(0.0, 0.0, 0.0));
            }
        }

        ComponentList<Torque> torques;
        p_transformaspect->GetComponentsByType<Torque>(torques);

        for (size_t i = 0; i < torques.size(); ++i)
        {
            Torque applied_torque = torques[i]->getPurpose();

            if (applied_torque.m_enabled)
            {
                Vector torque_v = applied_torque.m_torque_axis;
                torque_v.scale(applied_torque.m_torque_scale);

                Vector final_torque_v;

                if (Torque::LOCALE == applied_torque.m_mode)
                {
                    Matrix local_rot = local_transf;
                    local_rot.clearTranslation();
                    local_rot.transform(&torque_v, &final_torque_v);
                }
                else
                {
                    final_torque_v = torque_v;
                }

                m_rigidBody->applyTorque(btVector3(final_torque_v[0], final_torque_v[1], final_torque_v[2]));
            }
        }

        ComponentList<Matrix> mats;
        p_transformaspect->GetComponentsByType<Matrix>(mats);

        mats[0]->getPurpose() = local_transf;
        m_last_local_transf = local_transf;

        p_out_base_transform = local_transf * m_stack_matrix_inv;  // multiplier par l'inverse de la stack pour compenser le fait que local_transf renvoy�e par bullet inclu d�ja la stack matrix
                                                                    // (cf m�thode OnAddedInGraph() plus bas)
    }
}

btRigidBody* RigidBodyTransformAspectImpl::Init(TransformAspect* p_transformaspect)
{
    if (m_initialized)
    {
        _DSEXCEPTION("RigidBodyTransformAspectImpl already initialized !")
    }

    ComponentList<Matrix> mats;
    p_transformaspect->GetComponentsByType<Matrix>(mats);

    Matrix attitude_mat;
    if (mats.size())
    {
        attitude_mat = mats[0]->getPurpose();
    }
    else
    {
        attitude_mat.identity();
    }

    ComponentList<dsreal> reals;
    p_transformaspect->GetComponentsByType<dsreal>(reals);

    dsreal mass = 0.0;
    if (reals.size())
    {
        mass = reals[0]->getPurpose();
    }

    btScalar    btmat[16];
    btTransform bt_transform;

    convert_matrix_to_bt(attitude_mat, btmat);

    bt_transform.setFromOpenGLMatrix(btmat);

    //////////////////////////////////////////////////////////////////////////

    ComponentList<BoxCollisionShape> boxcollision_shapes;
    ComponentList<SphereCollisionShape> spherecollision_shapes;
    ComponentList<CompoundCollisionShape> compoundcollision_shapes;

    p_transformaspect->GetComponentsByType<BoxCollisionShape>(boxcollision_shapes);
    p_transformaspect->GetComponentsByType<SphereCollisionShape>(spherecollision_shapes);
    p_transformaspect->GetComponentsByType<CompoundCollisionShape>(compoundcollision_shapes);


    for (auto& e : boxcollision_shapes)
    {
        DrawSpace::Maths::Vector box_dims;
        box_dims = e->getPurpose().GetPos();

        btBoxShape* shape = _DRAWSPACE_NEW_(btBoxShape, btBoxShape(btVector3(box_dims[0], box_dims[1], box_dims[2])));

        Maths::Matrix transf = e->getPurpose().GetTransform();

        m_collisionShapesList.push_back(std::make_pair(shape, transf));
    }

    for (auto& e : spherecollision_shapes)
    {
        dsreal sphere_radius = spherecollision_shapes[0]->getPurpose().GetRay();;
        btSphereShape* shape = _DRAWSPACE_NEW_(btSphereShape, btSphereShape(sphere_radius));

        Maths::Matrix transf = e->getPurpose().GetTransform();

        m_collisionShapesList.push_back(std::make_pair(shape, transf));
    }

    // NOTE : no meshe collision here
    // Meshe collision shape are for pure colliders only !! (see bullet source code)

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

    /////////////////////////////////////////////////////////////////////////////////////////////////

    m_motionState = _DRAWSPACE_NEW_(btDefaultMotionState, btDefaultMotionState(bt_transform));

    btVector3 localInertia(0, 0, 0);
    if (mass > 0.0)
    {
        m_collisionShape->calculateLocalInertia(mass, localInertia);
    }

    btRigidBody::btRigidBodyConstructionInfo boxRigidBodyConstructionInfo(mass, m_motionState, m_collisionShape, localInertia);

    m_rigidBody = _DRAWSPACE_NEW_(btRigidBody, btRigidBody(boxRigidBodyConstructionInfo));
    m_rigidBody->setActivationState(DISABLE_DEACTIVATION);

    m_initialized = true;

    if (m_memorized_vectors)
    {
        m_rigidBody->setAngularVelocity(btVector3(m_mem_angularspeed[0], m_mem_angularspeed[1], m_mem_angularspeed[2]));
        m_rigidBody->setLinearVelocity(btVector3(m_mem_linearspeed[0], m_mem_linearspeed[1], m_mem_linearspeed[2]));

        m_memorized_vectors = false;
    }

    return m_rigidBody;
}

btRigidBody* RigidBodyTransformAspectImpl::GetRigidBody(void) const
{
    return m_rigidBody;
}

void RigidBodyTransformAspectImpl::RegisterPhysicalAspect(PhysicsAspect* p_physical_aspect)
{
    if (!m_initialized)
    {
        _DSEXCEPTION("bodyaspect not initialized")
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

void RigidBodyTransformAspectImpl::Release(void)
{
    if (!m_initialized)
    {
        _DSEXCEPTION("RigidBodyTransformAspectImpl not initialized !")
    }

    m_memorized_vectors = true;
    m_mem_linearspeed[0] = m_rigidBody->getLinearVelocity().x();
    m_mem_linearspeed[1] = m_rigidBody->getLinearVelocity().y();
    m_mem_linearspeed[2] = m_rigidBody->getLinearVelocity().z();
    m_mem_linearspeed[3] = 1.0;

    m_mem_angularspeed[0] = m_rigidBody->getAngularVelocity().x();
    m_mem_angularspeed[1] = m_rigidBody->getAngularVelocity().y();
    m_mem_angularspeed[2] = m_rigidBody->getAngularVelocity().z();
    m_mem_angularspeed[3] = 1.0;
    
    _DRAWSPACE_DELETE_(m_motionState);
    m_motionState = nullptr;

    for (auto& e : m_collisionShapesList)
    {
        _DRAWSPACE_DELETE_(e.first);
    }
    m_collisionShapesList.clear();

    if (m_compoundShape)
    {
        _DRAWSPACE_DELETE_(m_compoundShape);
        m_compoundShape = nullptr;
    }

    if (m_world)
    {
        m_world->removeRigidBody(m_rigidBody);

        m_world = nullptr;
        m_physical_aspect_owner = nullptr;
    }

    _DRAWSPACE_DELETE_(m_rigidBody);
    m_rigidBody = nullptr;


    m_initialized = false;
}

void RigidBodyTransformAspectImpl::convert_matrix_to_bt(const Matrix& p_mat, btScalar* bt_matrix)
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

void RigidBodyTransformAspectImpl::convert_matrix_from_bt(btScalar* bt_matrix, Maths::Matrix& p_mat)
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

void RigidBodyTransformAspectImpl::OnAddedInGraph(DrawSpace::Aspect::TransformAspect* p_transformaspect, const Matrix& p_parent_transform)
{
    Vector angularspeed_mem, linearspeed_mem;
    angularspeed_mem = m_mem_angularspeed;
    linearspeed_mem = m_mem_linearspeed;

    Matrix parent_transform{ p_parent_transform };

    parent_transform.clearTranslation();
    parent_transform.inverse();

    parent_transform.transform(&angularspeed_mem, &m_mem_angularspeed);
    parent_transform.transform(&linearspeed_mem, &m_mem_linearspeed);


    Matrix stack_mat;
    p_transformaspect->GetStackMatrix(stack_mat);

    ComponentList<Matrix> mats;
    p_transformaspect->GetComponentsByType<Matrix>(mats);
    Matrix current_attitude{ mats[0]->getPurpose() };

    Matrix new_local_mat;

    new_local_mat = current_attitude * stack_mat;

    // new inital attitude for rigid body;
    mats[0]->getPurpose() = new_local_mat;

    m_stack_matrix_inv = stack_mat;
    m_stack_matrix_inv.inverse();
}

void RigidBodyTransformAspectImpl::OnRemovedFromGraph(DrawSpace::Aspect::TransformAspect* p_transformaspect, const Matrix& p_parent_transform)
{

    Vector angularspeed_mem, linearspeed_mem;
    angularspeed_mem = m_mem_angularspeed;
    linearspeed_mem = m_mem_linearspeed;

    Matrix parent_transform{ p_parent_transform };

    parent_transform.clearTranslation();
    
    parent_transform.transform(&angularspeed_mem, &m_mem_angularspeed);
    parent_transform.transform(&linearspeed_mem, &m_mem_linearspeed);



    Matrix stack_mat;
    p_transformaspect->GetStackMatrix(stack_mat);

    stack_mat.inverse();

    ComponentList<Matrix> mats;
    p_transformaspect->GetComponentsByType<Matrix>(mats);
    Matrix current_attitude{ mats[0]->getPurpose() };

    Matrix new_local_mat;

    new_local_mat = current_attitude * stack_mat;

    // new inital attitude for rigid body;
    mats[0]->getPurpose() = new_local_mat;

    m_stack_matrix_inv = stack_mat;
    m_stack_matrix_inv.inverse();
}

DrawSpace::Maths::Matrix RigidBodyTransformAspectImpl::GetLastLocalTransform(void) const
{
    return m_last_local_transf;
}
