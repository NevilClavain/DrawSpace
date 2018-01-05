/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

class Body;

class World
{
protected:

    btDefaultCollisionConfiguration*            m_collisionConfiguration;
    btCollisionDispatcher*                      m_collisionDispatcher;
    btBroadphaseInterface*                      m_broadphase;
    btSequentialImpulseConstraintSolver*        m_sequentialImpulseConstraintSolver;

    btDiscreteDynamicsWorld*                    m_world;

    //std::vector<Body*>                          m_bodies;
    std::map<btRigidBody*, Body*>               m_bodies;

   
public:
    static dsreal                               m_scale;

public:
    World( void );
    virtual ~World( void );

    bool Initialize( void );
    bool SetGravity( const DrawSpace::Utils::Vector& p_gravity );
    bool GetGravity( DrawSpace::Utils::Vector& p_gravity );

    bool StepSimulation( dsreal p_fps, int p_nbsteps );

    btDynamicsWorld* getBulletWorld( void );

    //dsreal GetScale( void );
    void SetScale( dsreal p_scale );

    void AddBody( Body* p_body );
    void RemoveBody( Body* p_body );

    

};
}
}

#endif
