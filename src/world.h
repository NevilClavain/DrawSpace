/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#ifndef _WORLD_H_
#define _WORLD_H_

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "memalloc.h"
#include "vector.h"

namespace DrawSpace
{
namespace Dynamics
{
class World
{
protected:

    btDefaultCollisionConfiguration*            m_collisionConfiguration;
    btCollisionDispatcher*                      m_collisionDispatcher;
    btBroadphaseInterface*                      m_broadphase;
    btSequentialImpulseConstraintSolver*        m_sequentialImpulseConstraintSolver;

    btDiscreteDynamicsWorld*                    m_world;

public:
    World( void );
    virtual ~World( void );

    bool Initialize( void );
    bool SetGravity( const DrawSpace::Utils::Vector p_gravity );

    bool StepSimulation( long p_fps );

    btDynamicsWorld* getBulletWorld( void );

};
}
}

#endif
