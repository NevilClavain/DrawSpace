/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2021
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

#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "componentcontainer.h"
#include "matrix.h"
#include "meshe.h"

namespace DrawSpace
{
namespace Core
{
class Entity;
}

namespace Aspect
{
class PhysicsAspect;

class CollisionAspect : public Core::ComponentContainer
{
protected:
	bool                                                        m_initialized{ false };

	btDiscreteDynamicsWorld*									m_world{ nullptr };
	PhysicsAspect*												m_physical_aspect_owner{ nullptr };

	btTriangleMesh*												m_mesh{ nullptr };

	btRigidBody*												m_rigidBody{ nullptr };
	btDefaultMotionState*										m_motionState{ nullptr };

	std::vector<std::pair<btCollisionShape*, Utils::Matrix>>    m_collisionShapesList;
    btCollisionShape*                                           m_collisionShape{ nullptr };
    btCompoundShape*                                            m_compoundShape{ nullptr };

	void convert_matrix_to_bt(const Utils::Matrix& p_mat, btScalar* bt_matrix);
	void convert_matrix_from_bt(btScalar* bt_matrix, Utils::Matrix& p_mat);


public:

    struct BoxCollisionShape
    {
        BoxCollisionShape(const Utils::Vector& p_box) : m_box(p_box)
        {
            m_transformation.Identity();
        };

        BoxCollisionShape(const Utils::Vector& p_box, const Utils::Matrix& p_mat) :
            m_box(p_box),
            m_transformation(p_mat)
        {
        };

        Utils::Matrix GetTransform(void) const { return m_transformation; };
        Utils::Vector GetPos(void) const { return m_box; };

    private:
        Utils::Vector m_box;
        Utils::Matrix m_transformation;
    };

    struct SphereCollisionShape
    {
        SphereCollisionShape(dsreal p_ray) : m_ray(p_ray)
        {
            m_transformation.Identity();
        };

        SphereCollisionShape(dsreal p_ray, const Utils::Matrix& p_mat) :
            m_ray(p_ray),
            m_transformation(p_mat)
        {
        };

        Utils::Matrix GetTransform(void) const { return m_transformation; };

        dsreal GetRay(void) const { return m_ray; };
    private:
        dsreal m_ray;
        Utils::Matrix m_transformation;
    };

    struct MesheCollisionShape
    {
        MesheCollisionShape(const Core::Meshe& p_meshe) : m_meshe(p_meshe)
        {
            m_transformation.Identity();
        };
        MesheCollisionShape(const Core::Meshe& p_meshe, const Utils::Matrix& p_mat) :
            m_meshe(p_meshe),
            m_transformation(p_mat)
        {
            m_transformation.Identity();
        };

        Utils::Matrix GetTransform(void) const { return m_transformation; };

        Core::Meshe m_meshe;
    public:
        Utils::Matrix m_transformation;
    };

    struct CompoundCollisionShape
    {
        CompoundCollisionShape(void)
        {
            m_transformation.Identity();
        };

        Utils::Matrix GetTransform(void) const { return m_transformation; };

    private:
        Utils::Matrix m_transformation;
    };

	CollisionAspect(void);
	~CollisionAspect(void);

	btRigidBody*    Init(void);
    btRigidBody*    GetRigidBody(void) const;
    void            Update(Core::Entity* p_owner_entity);
	void            Release(void);

	void RegisterPhysicalAspect(PhysicsAspect* p_physical_aspect);

};
}
}
