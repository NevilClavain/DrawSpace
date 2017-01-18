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

#ifndef _ORBITER_H_
#define _ORBITER_H_

#include "body.h"
#include "chunk.h"
#include "timemanager.h"
#include "scenenode.h"

namespace DrawSpace
{
namespace Dynamics
{

class Centroid;

class Orbit
{
protected:

    dsreal                              m_orbit_angle;

    dsreal                              m_revolution_angle; // orbiter self rotation

    // orbit parameters
    dsreal                              m_ray; 
    dsreal                              m_excentricity;
    dsreal                              m_offset_angle;
    dsreal                              m_tilt_angle;       // inclinaison orbite
    dsreal                              m_offset_plane_x;
    dsreal                              m_offset_plane_y;

    dsreal                              m_revolution_tilt_angle; // inclinaison axe de rotation planete

    dsreal                              m_orbit_duration; // unit� : 1.0 = annee terrestre (365 jours)

    dsreal                              m_revolution_duration; // unite : 1.0 jour terrestre (24h)

    Centroid*                           m_centroid;
    DrawSpace::Chunk*                   m_drawable; // drawable representant la trajectoire orbite

    DrawSpace::Utils::Matrix            m_basetransform;



    void orbit_step( dsreal p_angle, DrawSpace::Utils::Matrix& p_orbit_mat, DrawSpace::Utils::Matrix& p_planet_mat );

    void build_orbit_meshe( dsreal p_anglestep, DrawSpace::Core::Meshe* p_meshe );

public:

    Orbit::Orbit( dsreal p_ray, dsreal p_excentricity, dsreal p_offset_angle, 
                    dsreal p_tilt_angle, dsreal p_offset_plane_x, dsreal p_offset_plane_y, dsreal p_orbit_duration,
                    dsreal p_revolution_tilt_angle, dsreal p_revolution_duration,
                    Centroid* p_centroid ) :
    m_ray( p_ray ),
    m_excentricity( p_excentricity ),
    m_offset_angle( p_offset_angle ),
    m_tilt_angle( p_tilt_angle ),
    m_offset_plane_x( p_offset_plane_x ),
    m_offset_plane_y( p_offset_plane_y ),
    m_drawable( NULL ),
    m_centroid( p_centroid ),
    m_orbit_angle( 0.0 ),
    m_orbit_duration( p_orbit_duration ),
    m_revolution_tilt_angle( p_revolution_tilt_angle ),
    m_revolution_duration( p_revolution_duration )
    {
        m_basetransform.Identity();
    };

    void    OrbitStep( const DrawSpace::Utils::Matrix& p_centroidbase );
    void    BuildMeshe( dsreal p_anglestep, DrawSpace::Core::Meshe* p_meshe );
    void    RegisterChunk( DrawSpace::Chunk* p_drawable );


    void    GetBaseTransform( DrawSpace::Utils::Matrix& p_mat );
    void    SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat ) { };
    
    void    OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node ) { };
    void    OnUnregister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node ) { };

    void    Update( DrawSpace::Utils::TimeManager& p_timemanager );
    void    Update2( DrawSpace::Utils::TimeManager& p_timemanager ) {};
    

    friend class Calendar;
};


class Orbiter : public Body
{
protected:

    btRigidBody*                                m_rigidBody;
    btCollisionShape*                           m_collisionShape;
    btTriangleMesh*                             m_meshe_data;
    btDefaultMotionState*                       m_motionState;

    dsreal                                      m_orbit_angle;
    dsreal                                      m_revolution_angle; // orbiter self rotation


    dsreal                                      m_orbit_duration; // unite : 1.0 = annee terrestre (365 jours)
    dsreal                                      m_revolution_duration; // unite : 1.0 jour terrestre (24h)
    dsreal                                      m_revolution_tilt_angle; // inclinaison axe de rotation planete

    DrawSpace::Utils::Matrix                    m_basetransform;

    //DrawSpace::Core::BaseSceneNode* m_owner;
    DrawSpace::Core::SceneNode<Orbiter>*        m_owner;


public:

    Orbiter( World* p_world );
    virtual ~Orbiter( void );

    void SetKinematic( const Body::Parameters& p_parameters );
    void UnsetKinematic( void );

    void AddToWorld( void );
    void RemoveFromWorld( void );

    virtual btRigidBody* GetRigidBody( void );

    void SetOrbitDuration( dsreal p_orbit_duration )
    {
        m_orbit_duration = p_orbit_duration;
    }

    void SetRevolutionDuration( dsreal p_revolution_duration )
    {
        m_revolution_duration = p_revolution_duration;
    }

    void SetRevolutionTiltAngle( dsreal p_angle )
    {
        m_revolution_tilt_angle = p_angle;
    }


    void Update( DrawSpace::Utils::TimeManager& p_timemanager );
    
    void GetBaseTransform( DrawSpace::Utils::Matrix& p_mat );
    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat );
    
    void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );
    void OnUnregister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );

    //DrawSpace::Core::BaseSceneNode* GetOwner( void );
    virtual DrawSpace::Core::SceneNode<Orbiter>* GetOwner( void );

    friend class Calendar;
};


class Centroid
{
protected:

    std::vector<Orbit*>         m_sub_orbits;
    DrawSpace::Utils::Matrix    m_transformation;
 
public:

    Centroid( void );
    void RegisterSubOrbit( Orbit* p_orbit );
    void Update( const DrawSpace::Utils::Matrix& p_prevcentroidbase, const DrawSpace::Utils::Matrix& p_localorbitmat, const DrawSpace::Utils::Matrix& p_localplanetmat );

    void GetBaseTransform( DrawSpace::Utils::Matrix& p_mat );
    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat ) {};
    
    void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node ) { };
    void OnUnregister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node ) { };


    void Update( DrawSpace::Utils::TimeManager& p_timemanager ) {};
    void Update2( DrawSpace::Utils::TimeManager& p_timemanager ) {};


};


}
}
#endif
