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
#include "configurable.h"


#define INERTBODY_TEXT_KEYWORD           "InertBody"
#define INERTBODY_ARC_MAGICNUMBER        0x6042


namespace DrawSpace
{
namespace Dynamics
{
class InertBody : public Body, public Core::Configurable
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

    DrawSpace::Core::TransformNode* m_drawable;

    void                            create_body( const btTransform& p_transform );
    void                            destroy_body( void );

    void init( void );
    void init_body( void );

    bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:

    InertBody( void );
    InertBody( World* p_world, DrawSpace::Core::TransformNode* p_drawable, const Body::Parameters& p_parameters );
    virtual ~InertBody( void );

    void SetWorld( World* p_world );

    void GetParameters( Parameters& p_parameters );
    void Update( void );

    void Attach( Body* p_body );
    void IncludeTo( Body* p_body, const DrawSpace::Utils::Matrix& p_initmat );
    void Detach( void );

    void GetLastLocalWorldTrans( DrawSpace::Utils::Matrix& p_mat );

    void ApplyForce( const DrawSpace::Utils::Vector p_force );

    dsreal GetLinearSpeedMagnitude( void );
    dsreal GetAngularSpeedMagnitude( void );

    void GetLinearSpeed( DrawSpace::Utils::Vector& p_speed );

    dsreal GetBoundingSphereRay( void );

    virtual btRigidBody* GetRigidBody( void );

    void GetTotalForce( DrawSpace::Utils::Vector& p_force );
    void GetTotalTorque( DrawSpace::Utils::Vector& p_torque );

    void RegisterEvtHandler( EventHandler* p_handler );

    bool HasLanded( void );

    bool IsActive( void );

    Body* GetRefBody( void );

    DrawSpace::Core::TransformNode* GetDrawable( void );
    void SetDrawable( DrawSpace::Core::TransformNode* p_drawable );


    void Serialize( Utils::Archive& p_archive  );
    bool Unserialize( Utils::Archive& p_archive );

    void DumpProperties( dsstring& p_text );
    bool ParseProperties( const dsstring& p_text );

    void ApplyProperties( void );

    static Configurable* Instanciate( void );    

};
}
}
#endif
