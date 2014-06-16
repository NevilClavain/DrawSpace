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

#ifndef _INERTBODY_H_
#define _INERTBODY_H_

#include "body.h"
#include "vector.h"
#include "matrix.h"

namespace DrawSpace
{
namespace Dynamics
{
class InertBody : public Body
{
protected:

    Parameters                      m_parameters;

    btRigidBody*                    m_rigidBody;
    btCollisionShape*               m_collisionShape;
    btTriangleMesh*                 m_meshe_data;
    btDefaultMotionState*           m_motionState;

    Body*                           m_refbody;
    World*                          m_global_world_mem;

    DrawSpace::Utils::Matrix        m_lastlocalworldtrans;

    void                            create_body( const btTransform& p_transform );
    void                            destroy_body( void );

public:

    InertBody( World* p_world, DrawSpace::Core::TransformNode* p_drawable, const Body::Parameters& p_parameters );
    virtual ~InertBody( void );

    void GetParameters( Parameters& p_parameters );
    void Update( void );

    void Attach( Body* p_body );
    void Detach( void );

    void GetLastLocalWorldTrans( DrawSpace::Utils::Matrix& p_mat );

    void ApplyForce( const DrawSpace::Utils::Vector p_force );

    dsreal GetLinearSpeedMagnitude( void );
    dsreal GetAngularSpeedMagnitude( void );

    virtual btRigidBody* GetRigidBody( void );

    void GetTotalForce( DrawSpace::Utils::Vector& p_force );
    void GetTotalTorque( DrawSpace::Utils::Vector& p_torque );

    void RegisterEvtHandler( EventHandler* p_handler );

    Body* GetRefBody( void );
};
}
}
#endif
