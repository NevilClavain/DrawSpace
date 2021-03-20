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
#include "transformaspectimpl.h"

namespace DrawSpace
{
namespace Aspect
{
class PhysicsAspect;
}
namespace AspectImplementations
{
class RigidBodyTransformAspectImpl : public DrawSpace::Interface::AspectImplementations::TransformAspectImpl
{
protected:

    btDiscreteDynamicsWorld*                                    m_world{ nullptr };
    Aspect::PhysicsAspect*                                      m_physical_aspect_owner{ nullptr };

    bool                                                        m_initialized{ false };
    btRigidBody*                                                m_rigidBody{ nullptr };

    btCollisionShape*                                           m_collisionShape{ false };
    std::vector<std::pair<btCollisionShape*, Utils::Matrix>>    m_collisionShapesList;

    btCompoundShape*                                            m_compoundShape{ nullptr };

    btDefaultMotionState*                                       m_motionState{ nullptr };


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

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class Force
    {
    public:
        typedef enum
        {
            GLOBAL,
            LOCALE
        } Mode;

    protected:
        DrawSpace::Utils::Vector    m_force_dir;
        dsreal                      m_force_scale;
        Mode                        m_mode;
        bool                        m_enabled;

    public:
        Force(const DrawSpace::Utils::Vector& p_dir, Mode p_mode = LOCALE, bool p_enabled = false) :
            m_force_dir(p_dir),
            m_mode(p_mode),
            m_enabled(p_enabled),
            m_force_scale(1.0) {}

        Force(void) :
            m_mode(LOCALE),
            m_enabled(false),
            m_force_scale(1.0) {}

        inline void UpdateForce(const DrawSpace::Utils::Vector& p_dir) { m_force_dir = p_dir; }
        inline void UpdateForceScale(dsreal p_scale) { m_force_scale = p_scale; }
        inline void Enable(void) { m_enabled = true; }
        inline void Disable(void) { m_enabled = false; }

        friend class RigidBodyTransformAspectImpl;
    };

    class Torque
    {
    public:
        typedef enum
        {
            GLOBAL,
            LOCALE
        } Mode;

    protected:
        DrawSpace::Utils::Vector    m_torque_axis;
        dsreal                      m_torque_scale;
        Mode                        m_mode;
        bool                        m_enabled;

    public:
        Torque(const DrawSpace::Utils::Vector& p_axis, Mode p_mode = LOCALE, bool p_enabled = false) :
            m_torque_axis(p_axis),
            m_mode(p_mode),
            m_enabled(p_enabled),
            m_torque_scale(1.0) {}

        Torque(void) :
            m_mode(LOCALE),
            m_enabled(false),
            m_torque_scale(1.0) {}

        inline void UpdateForce(const DrawSpace::Utils::Vector& p_axis) { m_torque_axis = p_axis; }
        inline void UpdateForceScale(dsreal p_scale) { m_torque_scale = p_scale; }
        inline void Enable(void) { m_enabled = true; }
        inline void Disable(void) { m_enabled = false; }

        friend class RigidBodyTransformAspectImpl;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    RigidBodyTransformAspectImpl( void );
    virtual void GetLocaleTransform(Aspect::TransformAspect* p_transformaspect, Utils::Matrix& p_out_base_transform);

    void Init(Aspect::TransformAspect* p_transformaspect);

    void RegisterPhysicalAspect(Aspect::PhysicsAspect* p_physical_aspect);

    void Release(void);
};
}
}
