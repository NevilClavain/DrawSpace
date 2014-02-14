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

namespace DrawSpace
{
namespace Dynamics
{
class Orbiter : public Body
{
public:

    typedef struct
    {       
        DrawSpace::Utils::Vector    initial_pos;
        DrawSpace::Utils::Matrix    inital_rot;

        Body::Shape                 shape;

        DrawSpace::Utils::Vector    box_dims;

    } Parameters;

    class Orbit
    {
    protected:

        // orbit parameters
        dsreal                      m_ray; 
        dsreal                      m_excentricity;
        dsreal                      m_offset_angle;
        dsreal                      m_tilt_angle;
        dsreal                      m_offset_plane_x;
        dsreal                      m_offset_plane_y;

    public:

        Orbit::Orbit( void ) :
        m_ray( 0.0 ),
        m_excentricity( 0.0 ),
        m_offset_angle( 0.0 ),
        m_tilt_angle( 0.0 ),
        m_offset_plane_x( 0.0 ),
        m_offset_plane_y( 0.0 )
        {
        };

        void Compute( dsreal p_angle, DrawSpace::Utils::Vector& p_respoint );
    };

protected:

    btRigidBody*                    m_rigidBody;
    btCollisionShape*               m_collisionShape;
    btDefaultMotionState*           m_motionState;

    DrawSpace::Utils::Vector        m_centroid;

    dsreal                          m_angle;

    Orbiter*                        m_parent;
    std::vector<Orbiter*>           m_children;

    Orbit                           m_orbit_1;
    Orbit                           m_orbit_2;

public:

    Orbiter( World* p_world, DrawSpace::Interface::Drawable* p_drawable );
    virtual ~Orbiter( void );

    bool SetKinematic( const Parameters& p_parameters );
    bool UnsetKinematic( void );


    void Update( const DrawSpace::Utils::Vector& p_centroid );

    void AddChild( Orbiter* p_orbiter );

};
}
}

#endif
