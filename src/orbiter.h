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

#ifndef _ORBITER_H_
#define _ORBITER_H_

#include "body.h"
#include "timemanager.h"

namespace DrawSpace
{
namespace Dynamics
{

class Centroid;

class Orbit
{
protected:

    dsreal                              m_orbit_angle;

    // orbit parameters
    dsreal                              m_ray; 
    dsreal                              m_excentricity;
    dsreal                              m_offset_angle;
    dsreal                              m_tilt_angle;
    dsreal                              m_offset_plane_x;
    dsreal                              m_offset_plane_y;
    Centroid*                           m_centroid;

    dsreal                              m_orbit_duration;
    
    DrawSpace::Drawable*                m_drawable; // drawable representant la trajectoire orbite

    void orbit_step( dsreal p_angle, DrawSpace::Utils::Matrix& p_mat );

    void build_orbit_meshe( dsreal p_anglestep, DrawSpace::Core::Meshe* p_meshe );

public:

    Orbit::Orbit( dsreal p_ray, dsreal p_excentricity, dsreal p_offset_angle, 
                    dsreal p_tilt_angle, dsreal p_offset_plane_x, dsreal p_offset_plane_y, dsreal p_orbit_duration,
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
    m_orbit_duration( p_orbit_duration )
    {
    };

    void    OrbitStep( const DrawSpace::Utils::Matrix& p_centroidbase );
    void    BuildMeshe( dsreal p_anglestep, DrawSpace::Core::Meshe* p_meshe );
    void    RegisterDrawable( DrawSpace::Drawable* p_drawable );

    void    Progress( DrawSpace::Utils::TimeManager& p_timer );

    friend class Calendar;
};


class Orbiter : public Body
{
protected:

    btRigidBody*                    m_rigidBody;
    btCollisionShape*               m_collisionShape;
    btDefaultMotionState*           m_motionState;

public:

    Orbiter( World* p_world, DrawSpace::Drawable* p_drawable );
    virtual ~Orbiter( void );

    void Update( const DrawSpace::Utils::Matrix& p_mat );

    void SetKinematic( const Body::Parameters& p_parameters );
    void UnsetKinematic( void );
};


class Centroid
{
protected:

    Orbiter*                    m_orbiter;
    std::vector<Orbit*>         m_sub_orbits;
    DrawSpace::Utils::Matrix    m_transformation;
 
public:

    Centroid( void );
    void RegisterSubOrbit( Orbit* p_orbit );
    void Update( const DrawSpace::Utils::Matrix& p_prevcentroidbase, const DrawSpace::Utils::Matrix& p_localorbitmat );
    void SetOrbiter( Orbiter* p_orbiter );
};


}
}
#endif
