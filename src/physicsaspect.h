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

#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "componentcontainer.h"
#include "entity.h"
#include "timeaspect.h"

namespace DrawSpace
{
namespace AspectImplementations
{
    class RigidBodyTransformAspectImpl;
}
namespace Aspect
{
class PhysicsAspect : public Core::ComponentContainer
{
protected:

    btDefaultCollisionConfiguration*                                m_collisionConfiguration;
    btCollisionDispatcher*                                          m_collisionDispatcher;
    btDbvtBroadphase*                                               m_broadphase;
    btSequentialImpulseConstraintSolver*                            m_sequentialImpulseConstraintSolver;
    btDiscreteDynamicsWorld*                                        m_world;

    bool                                                            m_gravity_applied;

    TimeAspect*                                                     m_time_aspect;

    std::map<btRigidBody*, DrawSpace::Core::Entity*>                m_bodies; // entites gerées "sous" l'entite-world physique possedant cet aspect

    AspectImplementations::RigidBodyTransformAspectImpl*            find_rigidbodytransformaspectimpl(DrawSpace::Core::Entity* p_entity) const;

public:

    PhysicsAspect( void );
    ~PhysicsAspect( void );
    void StepSimulation( void );

    void SetTimeAspect( TimeAspect* p_time_aspect );
   
    btDiscreteDynamicsWorld* GetWorld( void ) const;

    void RegisterRigidBody(DrawSpace::Core::Entity* p_entity);
    void UnregisterRigidBody(DrawSpace::Core::Entity* p_entity);

    void RegisterCollider(DrawSpace::Core::Entity* p_entity);
    void UnregisterCollider(DrawSpace::Core::Entity* p_entity);

};
}
}

