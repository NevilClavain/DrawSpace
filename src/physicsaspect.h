/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _PHYSICSASPECT_H_
#define _PHYSICSASPECT_H_

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "aspect.h"
#include "entity.h"

namespace DrawSpace
{
namespace Aspect
{
class PhysicsAspect : public Core::Aspect
{
protected:

    btDefaultCollisionConfiguration                                 m_collisionConfiguration;
    btCollisionDispatcher                                           m_collisionDispatcher;
    btDbvtBroadphase                                                m_broadphase;
    btSequentialImpulseConstraintSolver                             m_sequentialImpulseConstraintSolver;
    btDiscreteDynamicsWorld                                         m_world;

    bool                                                            m_gravity_applied;

    std::map<btRigidBody*, DrawSpace::Core::Entity*>                m_bodies;
    std::set<DrawSpace::Core::Entity*>                              m_bodies_set; // les memes entities que dans m_bodies

    void on_added_bodyentity( DrawSpace::Core::Entity* p_entity );
    void on_removed_bodyentity( DrawSpace::Core::Entity* p_entity );

public:

    PhysicsAspect( void );
    void StepSimulation( dsreal p_fps, int p_nbsteps );
    void UpdateBodiesList( const std::set<DrawSpace::Core::Entity*>& p_list );
};
}
}

#endif