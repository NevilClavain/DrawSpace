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

#include "spectatormovement.h"
#include "maths.h"
#include "misc_utils.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


SpectatorMovement::SpectatorMovement( void ) :
m_attachedbody( NULL ),
m_linked_to_orbiter( false ),
m_time_manager( NULL )
{
    m_timercb = _DRAWSPACE_NEW_( SpectatorTimer, SpectatorTimer( this, &SpectatorMovement::on_timer ) );
    m_timer.SetHandler( m_timercb );
}

SpectatorMovement::~SpectatorMovement( void )
{
    if( m_time_manager )
    {
        m_time_manager->UnregisterTimer( &m_timer );
    }
    _DRAWSPACE_DELETE_( m_timercb );
}

void SpectatorMovement::compute_pos( void )
{
    Vector body_speed;
    m_attachedbody->GetLinearSpeed( body_speed );

    Vector body_speed_2;

    Vector camera_position;
    Vector direction;

    Matrix body_transf;
    Matrix body_transf_inv;

    if( m_linked_to_orbiter )
    {
        m_attachedbody->GetLastLocalWorldTrans( body_transf );
    }
    else
    {
        m_attachedbody->GetLastWorldTransformation( body_transf );
    }

    body_transf_inv = body_transf;
    body_transf_inv.ClearTranslation();
    body_transf_inv.Inverse();

    body_transf_inv.Transform( &body_speed, &body_speed_2 );



    dsreal ray = m_attachedbody->GetBoundingSphereRay();
    ray *= m_scalepos;

    if( 0.0 == body_speed_2.LengthPow2() )
    {
        direction[0] = 0.5;
        direction[1] = 0.0;
        direction[2] = -1.0;

        direction.Normalize();
        direction.Scale( ray );
    }    
    else
    {
        Vector n_body_speed = body_speed_2;
        n_body_speed.Normalize();

        Vector y_axis( 0.0, 1.0, 0.0, 1.0 );
        Vector z_axis( 0.0, 0.0, 1.0, 1.0 );
        Vector lateral;

        if( y_axis * n_body_speed > 0.5 )
        {
            // vitesse presque colineaire a l'axe y
            lateral = ProdVec( z_axis, n_body_speed );
        }
        else
        {
            // vitesse pas vraiment colineaire a l'axe y
            lateral = ProdVec( y_axis, n_body_speed );                       
        }

        lateral.Normalize();
        lateral.Scale( 0.25 );

        direction[0] = n_body_speed[0] + lateral[0];
        direction[1] = n_body_speed[1] + lateral[1];
        direction[2] = n_body_speed[2] + lateral[2];

        direction.Normalize();
        direction.Scale( ray );

    }



    Matrix translate;

    translate.Translation( direction );
     
    m_result = translate * body_transf;
}

void SpectatorMovement::SetName( const dsstring& p_name )
{
    m_name = p_name;
}

void SpectatorMovement::Init( dsreal p_scalepos, long p_posperiod, bool p_orbiterlink )
{
    m_linked_to_orbiter = p_orbiterlink;

    m_scalepos = p_scalepos;
    m_posperiod = p_posperiod;
    m_timer.SetPeriod( m_posperiod );

    // pour executer compute_pos() des le 1er appel � SpectatorMovement::Compute(); les appels suivants seront fait periodiquement
    // sur appel timer manager a on_timer()
    m_compute = true;   
}

void SpectatorMovement::SetRefBody( DrawSpace::Dynamics::InertBody* p_refbody )
{
    m_attachedbody = p_refbody;
}

void SpectatorMovement::Compute( TimeManager& p_timemanager )
{
    if( !m_time_manager )
    {
        m_time_manager = &p_timemanager;
        p_timemanager.RegisterTimer( &m_timer );
    }
    
    if( m_compute )
    {
        /*
        p_timemanager.AddTimer( timer_name, m_posperiod, m_timercb );
        p_timemanager.SetTimerState( timer_name, true );
        */
                       
        m_timer.SetState( true );
        compute_pos();
        m_compute = false;
    }    
}

void SpectatorMovement::on_timer( Timer* p_timer )
{
    m_compute = true;
}

dsreal SpectatorMovement::GetScalePos( void )
{
    return m_scalepos;
}

long SpectatorMovement::GetPosPeriod( void )
{
    return m_posperiod;
}

bool SpectatorMovement::GetOrbiterLinkState( void )
{
    return m_linked_to_orbiter;
}

void SpectatorMovement::Update( DrawSpace::Utils::TimeManager& p_timemanager )
{
    Compute( p_timemanager );
}

void SpectatorMovement::GetBaseTransform( Matrix& p_mat )
{
    p_mat = m_result;
}
