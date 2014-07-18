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

#include "headmovement.h"
#include "maths.h"
#include "transformation.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;

HeadMovement::HeadMovement( void ) :
m_scalefactor( 1.0 ),
m_refbody( NULL ),
m_body_mass( 1.0 )
{
}

HeadMovement::~HeadMovement( void )
{
}

void HeadMovement::Init( DrawSpace::Dynamics::InertBody* p_refbody, dsreal p_scalefactor, dsreal p_ref_force, const Vector& p_head_pos )
{
    m_refbody = p_refbody;
    m_scalefactor = p_scalefactor;

    InertBody::Parameters params;

    m_refbody->GetParameters( params );
    m_body_mass = params.mass;

    m_head_pos = p_head_pos;
    m_ref_force = p_ref_force;


    m_player_view_linear_acc[0] = m_player_view_linear_acc[1] = m_player_view_linear_acc[2] = 0;
    m_player_view_linear_acc_2[0] = m_player_view_linear_acc_2[1] = m_player_view_linear_acc_2[2] = 0;
    m_player_view_linear_speed[0] = m_player_view_linear_speed[1] = m_player_view_linear_speed[2] = 0;

    m_player_view_angular_acc[0] = m_player_view_angular_acc[1] = m_player_view_angular_acc[2] = 0;
    m_player_view_angular_acc_2[0] = m_player_view_angular_acc_2[1] = m_player_view_angular_acc_2[2] = 0;
    m_player_view_angular_speed[0] = m_player_view_angular_speed[1] = m_player_view_angular_speed[2] = 0;


    m_player_view_angular_speed_clamp_up[0] = 0.0;
    m_player_view_angular_speed_clamp_down[0] = 0.0;

    m_player_view_angular_speed_clamp_up[1] = 0.0;
    m_player_view_angular_speed_clamp_down[1] = 0.0;

    m_player_view_angular_speed_clamp_up[2] = 0.0;
    m_player_view_angular_speed_clamp_down[2] = 0.0;


    m_player_view_linear_speed_clamp_up[0] = 0.0;
    m_player_view_linear_speed_clamp_down[0] = 0.0;

    m_player_view_linear_speed_clamp_up[1] = 0.0;
    m_player_view_linear_speed_clamp_down[1] = 0.0;

    m_player_view_linear_speed_clamp_up[2] = 0.0;
    m_player_view_linear_speed_clamp_down[2] = 0.0;



    m_player_view_theta = m_player_view_phi = m_player_view_rho = 0.0;


}

void HeadMovement::Compute( DrawSpace::Utils::TimeManager& p_timemanager )
{
    m_result.Identity();

    Vector tf, tt;
    Vector tf2, tt2;

    Matrix body_transf;

    m_refbody->GetLastLocalWorldTrans( body_transf );
    body_transf.ClearTranslation();
    body_transf.Inverse();

    m_refbody->GetTotalForce( tf );
    m_refbody->GetTotalTorque( tt );
         
    body_transf.Transform( &tf, &tf2 );
    body_transf.Transform( &tt, &tt2 );

    m_player_view_linear_acc[2] = -tf2[2] / m_body_mass;

    m_player_view_angular_acc[2] = -tt2[2];
    m_player_view_angular_acc[1] = -tt2[1];
    m_player_view_angular_acc[0] = -tt2[0];

    /////////////////////////////////////////


    //Matrix cam_base_pos;
    m_cam_base_pos.Translation( m_head_pos );


    /////////////////////////////////////////////////////

    if( m_player_view_linear_acc[2] > 0.0 )
    {
        m_player_view_linear_speed_clamp_up[2] = 10.0;
        m_player_view_linear_speed_clamp_down[2] = 0.0;

        dsreal limit = ( m_player_view_linear_acc[2] / ( m_ref_force / m_body_mass ) );

        if( m_player_view_pos[2] < limit )
        {
            m_player_view_linear_acc_2[2] = 5.0;
        }
        else
        {
            m_player_view_linear_acc_2[2] = -5.0;
        }
    }
    else if( m_player_view_linear_acc[2] < 0.0 )
    {
        m_player_view_linear_speed_clamp_up[2] = 0.0;
        m_player_view_linear_speed_clamp_down[2] = -10.0;

        dsreal limit = ( m_player_view_linear_acc[2] / ( m_ref_force / m_body_mass ) );

        if( m_player_view_pos[2] > limit )
        {
            m_player_view_linear_acc_2[2] = -5.0;
        }
        else
        {
            m_player_view_linear_acc_2[2] = 5.0;
        }
    }
    else
    {
        if( m_player_view_pos[2] > 0.01 )
        {
            m_player_view_linear_speed_clamp_up[2] = 0.0;
            m_player_view_linear_speed_clamp_down[2] = -10.0;

            m_player_view_linear_acc_2[2] = -5.0;
        }

        else if( m_player_view_pos[2] < -0.01 )
        {
            m_player_view_linear_speed_clamp_up[2] = 10.0;
            m_player_view_linear_speed_clamp_down[2] = 0.0;

            m_player_view_linear_acc_2[2] = 5.0;
        }
        else
        {
            if( m_player_view_pos[2] > 0.0 )
            {
                m_player_view_linear_acc_2[2] = 5.0;

                m_player_view_linear_speed_clamp_up[2] = 0.0;
                m_player_view_linear_speed_clamp_down[2] = -10.0;
            }
            else if( m_player_view_pos[2] < 0.0 )
            {
                m_player_view_angular_acc_2[2] = -5.0;

                m_player_view_linear_speed_clamp_up[2] = 10.0;
                m_player_view_linear_speed_clamp_down[2] = 0.0;
            }
        }
    }

    // ICI
    /////////////////////////////////////////////////////


    dsreal zpos = m_player_view_pos[2];
    p_timemanager.TranslationSpeedInc( &zpos, m_player_view_linear_speed[2] );
    m_player_view_pos[2] = zpos;


    dsreal curr_speed = m_player_view_linear_speed[2];
    p_timemanager.TranslationSpeedInc( &curr_speed, m_player_view_linear_acc_2[2] );
    m_player_view_linear_speed[2] = curr_speed;   
    m_player_view_linear_speed[2] = DrawSpace::Utils::Maths::Clamp( m_player_view_linear_speed_clamp_down[2], m_player_view_linear_speed_clamp_up[2], m_player_view_linear_speed[2] );


    m_player_view_pos.Scale( m_scalefactor );

    //Matrix cam_delta_pos;
    m_cam_delta_pos.Translation( m_player_view_pos );

    /////////////////////////////////////////////////////

    if( m_player_view_angular_acc[0] > 0.0 )
    {
        m_player_view_angular_speed_clamp_up[0] = 10.0;
        m_player_view_angular_speed_clamp_down[0] = 0.0;

        if( m_player_view_phi < 4 * m_player_view_angular_acc[0] )
        {
            m_player_view_angular_acc_2[0] = 5.0;
        }
        else
        {
            m_player_view_angular_acc_2[0] = -5.0;
        }
    }
    else if( m_player_view_angular_acc[0] < 0.0 )
    {
        m_player_view_angular_speed_clamp_up[0] = 0.0;
        m_player_view_angular_speed_clamp_down[0] = -10.0;

        if( m_player_view_phi > 4 * m_player_view_angular_acc[0] )
        {
            m_player_view_angular_acc_2[0] = -5.0;
        }
        else
        {
            m_player_view_angular_acc_2[0] = 5.0;
        }
    }
    else
    {
        if( m_player_view_phi > 0.3 )
        {
            m_player_view_angular_speed_clamp_up[0] = 0.0;
            m_player_view_angular_speed_clamp_down[0] = -10.0;

            m_player_view_angular_acc_2[0] = -5.0;
        }

        else if( m_player_view_phi < -0.3 )
        {
            m_player_view_angular_speed_clamp_up[0] = 10.0;
            m_player_view_angular_speed_clamp_down[0] = 0.0;

            m_player_view_angular_acc_2[1] = 5.0;
        }

        else
        {
            if( m_player_view_phi > 0.0 )
            {
                m_player_view_angular_acc_2[0] = 5.0;

                m_player_view_angular_speed_clamp_up[0] = 0.0;
                m_player_view_angular_speed_clamp_down[0] = -10.0;
            }
            else if( m_player_view_phi < 0.0 )
            {
                m_player_view_angular_acc_2[0] = -5.0;

                m_player_view_angular_speed_clamp_up[0] = 10.0;
                m_player_view_angular_speed_clamp_down[0] = 0.0;
            }
        }
    }






    if( m_player_view_angular_acc[1] > 0.0 )
    {
        m_player_view_angular_speed_clamp_up[1] = 10.0;
        m_player_view_angular_speed_clamp_down[1] = 0.0;

        if( m_player_view_theta < 4 * m_player_view_angular_acc[1] )
        {
            m_player_view_angular_acc_2[1] = 5.0;
        }
        else
        {
            m_player_view_angular_acc_2[1] = -5.0;
        }
    }
    else if( m_player_view_angular_acc[1] < 0.0 )
    {
        m_player_view_angular_speed_clamp_up[1] = 0.0;
        m_player_view_angular_speed_clamp_down[1] = -10.0;

        if( m_player_view_theta > 4 * m_player_view_angular_acc[1] )
        {
            m_player_view_angular_acc_2[1] = -5.0;
        }
        else
        {
            m_player_view_angular_acc_2[1] = 5.0;
        }
    }
    else
    {
        if( m_player_view_theta > 0.3 )
        {
            m_player_view_angular_speed_clamp_up[1] = 0.0;
            m_player_view_angular_speed_clamp_down[1] = -10.0;

            m_player_view_angular_acc_2[1] = -5.0;
        }

        else if( m_player_view_theta < -0.3 )
        {
            m_player_view_angular_speed_clamp_up[1] = 10.0;
            m_player_view_angular_speed_clamp_down[1] = 0.0;

            m_player_view_angular_acc_2[1] = 5.0;
        }

        else
        {
            if( m_player_view_theta > 0.0 )
            {
                m_player_view_angular_acc_2[1] = 5.0;

                m_player_view_angular_speed_clamp_up[1] = 0.0;
                m_player_view_angular_speed_clamp_down[1] = -10.0;
            }
            else if( m_player_view_theta < 0.0 )
            {
                m_player_view_angular_acc_2[1] = -5.0;

                m_player_view_angular_speed_clamp_up[1] = 10.0;
                m_player_view_angular_speed_clamp_down[1] = 0.0;
            }            
        }
    }





    if( m_player_view_angular_acc[2] > 0.0 )
    {
        m_player_view_angular_speed_clamp_up[2] = 10.0;
        m_player_view_angular_speed_clamp_down[2] = 0.0;

        if( m_player_view_rho < 3 * m_player_view_angular_acc[2] )
        {
            m_player_view_angular_acc_2[2] = 5.0;
        }
        else
        {
            m_player_view_angular_acc_2[2] = -5.0;
        }
    }
    else if( m_player_view_angular_acc[2] < 0.0 )
    {
        m_player_view_angular_speed_clamp_up[2] = 0.0;
        m_player_view_angular_speed_clamp_down[2] = -10.0;

        if( m_player_view_rho > 3 * m_player_view_angular_acc[2] )
        {          
            m_player_view_angular_acc_2[2] = -5.0;
        }
        else
        {
            m_player_view_angular_acc_2[2] = 5.0;
        }
    }
    else
    {
        if( m_player_view_rho > 0.3 )
        {
            m_player_view_angular_speed_clamp_up[2] = 0.0;
            m_player_view_angular_speed_clamp_down[2] = -10.0;

            m_player_view_angular_acc_2[2] = -5.0;
        }

        else if( m_player_view_rho < -0.3 )
        {
            m_player_view_angular_speed_clamp_up[2] = 10.0;
            m_player_view_angular_speed_clamp_down[2] = 0.0;

            m_player_view_angular_acc_2[2] = 5.0;
        }

        else
        {
            if( m_player_view_rho > 0.0 )
            {
                m_player_view_angular_acc_2[2] = 5.0;

                m_player_view_angular_speed_clamp_up[2] = 0.0;
                m_player_view_angular_speed_clamp_down[2] = -10.0;
            }
            else if( m_player_view_rho < 0.0 )
            {
                m_player_view_angular_acc_2[2] = -5.0;

                m_player_view_angular_speed_clamp_up[2] = 10.0;
                m_player_view_angular_speed_clamp_down[2] = 0.0;
            }
        }
    }


    // ICI


    dsreal phi = m_player_view_phi;
    p_timemanager.TranslationSpeedInc( &phi, m_player_view_angular_speed[0] );
    m_player_view_phi = phi;


    dsreal rho = m_player_view_rho;
    p_timemanager.TranslationSpeedInc( &rho, m_player_view_angular_speed[2] );
    m_player_view_rho = rho;


    dsreal theta = m_player_view_theta;
    p_timemanager.TranslationSpeedInc( &theta, m_player_view_angular_speed[1] );
    m_player_view_theta = theta;




    curr_speed = m_player_view_angular_speed[0];
    p_timemanager.TranslationSpeedInc( &curr_speed, m_player_view_angular_acc_2[0] );
    m_player_view_angular_speed[0] = curr_speed;

    
    m_player_view_angular_speed[0] = DrawSpace::Utils::Maths::Clamp( m_player_view_angular_speed_clamp_down[0], m_player_view_angular_speed_clamp_up[0], m_player_view_angular_speed[0] );




    curr_speed = m_player_view_angular_speed[1];
    p_timemanager.TranslationSpeedInc( &curr_speed, m_player_view_angular_acc_2[1] );
    m_player_view_angular_speed[1] = curr_speed;

    
    m_player_view_angular_speed[1] = DrawSpace::Utils::Maths::Clamp( m_player_view_angular_speed_clamp_down[1], m_player_view_angular_speed_clamp_up[1], m_player_view_angular_speed[1] );



    
    curr_speed = m_player_view_angular_speed[2];
    p_timemanager.TranslationSpeedInc( &curr_speed, m_player_view_angular_acc_2[2] );
    m_player_view_angular_speed[2] = curr_speed;

    
    m_player_view_angular_speed[2] = DrawSpace::Utils::Maths::Clamp( m_player_view_angular_speed_clamp_down[2], m_player_view_angular_speed_clamp_up[2], m_player_view_angular_speed[2] );



    
    /////////////////////////////////////////////////////

    //Matrix rotx;
    m_rotx.Rotation( Vector( 1.0, 0.0, 0.0, 1.0 ), DrawSpace::Utils::Maths::DegToRad( m_player_view_phi * m_scalefactor ) );

    //Matrix roty;
    m_roty.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), DrawSpace::Utils::Maths::DegToRad( m_player_view_theta * m_scalefactor ) );

    //Matrix rotz;
    m_rotz.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), DrawSpace::Utils::Maths::DegToRad( m_player_view_rho * m_scalefactor ) );

    

    Transformation trf;

    trf.ClearAll();
    trf.PushMatrix( m_cam_delta_pos );
    trf.PushMatrix( m_cam_base_pos );
    trf.PushMatrix( m_rotx );
    trf.PushMatrix( m_roty );
    trf.PushMatrix( m_rotz );
    trf.BuildResult();
    trf.GetResult( &m_result );
               
}
