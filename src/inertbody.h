/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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
#include "scenenodegraph.h"

namespace DrawSpace
{
namespace Dynamics
{
class InertBody : public Body
{
protected:

    typedef enum
    {
        BODY_INITIAL_ATTITUDE_TRANSLATION,
        BODY_INITIAL_ATTITUDE_ROTATION,

    } BodyInitialAttitudComponentType;

    typedef struct
    {
        BodyInitialAttitudComponentType type;
        DrawSpace::Utils::Vector        vector; // si rotation : axe de rotation; si translation : composantes translation
        dsreal                          angle;

    } BodyInitialAttitudComponent;

    Parameters                      m_parameters;

    btRigidBody*                    m_rigidBody;
    btCollisionShape*               m_collisionShape;
    btTriangleMesh*                 m_meshe_data;
    btDefaultMotionState*           m_motionState;

    Body*                           m_attachedbody;
    World*                          m_global_world_mem;

    DrawSpace::Utils::Matrix        m_lastlocalworldtrans;
   

    DrawSpace::Core::BaseSceneNode* m_owner;

    // attribut d'infos pour les planetes et stations....
    bool                            m_enable_dynamiclink;
    bool                            m_enable_dynamiclink_initstate;
    Body*                           m_referent_body;
    DrawSpace::Utils::Matrix        m_dynamiclink_initial_matrix;


    void                            create_body( const btTransform& p_transform );
    void                            destroy_body( void );

    void init( void );
    void init_body( void );

public:

    InertBody( void );
    InertBody( World* p_world, const Body::Parameters& p_parameters );
    virtual ~InertBody( void );

    void SetWorld( World* p_world );

    bool IsDynamicLinkEnabled( void );
    void DisableDynamicLink( void );
    void SetDynamicLinkInitState( bool p_state );
    bool IsDynamicLinkInitState( void );
    void SetDynamicLinkInitialMatrix( const DrawSpace::Utils::Matrix& p_mat );
    void GetDynamicLinkInitialMatrix( DrawSpace::Utils::Matrix& p_mat );
    void SetReferentBody( Body* p_body );
    Body* GetReferentBody( void );

    void GetParameters( Parameters& p_parameters );
    void Update( DrawSpace::Utils::TimeManager& p_timemanager );

    
    void Attach( Body* p_body );
    void IncludeTo( Body* p_body );
    void Detach( void );


    void ForceInitialAttitude( const DrawSpace::Utils::Matrix& p_mat );

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

    Body* GetAttachedBody( void );

    void GetBaseTransform( DrawSpace::Utils::Matrix& p_mat );

    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat ) { };

    DrawSpace::Core::BaseSceneNode* GetOwner( void );
    
    void Enable( bool p_state );
    bool IsEnabled( void );

    void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );

    

};
}
}
#endif
