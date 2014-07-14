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

#include "spectatormovement.h"
#include "maths.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


SpectatorMovement::SpectatorMovement( void ) :
m_attachedbody( NULL )
{
    m_timercb = _DRAWSPACE_NEW_( SpectatorTimer, SpectatorTimer( this, &SpectatorMovement::on_timer ) );
}

SpectatorMovement::~SpectatorMovement( void )
{
    _DRAWSPACE_DELETE_( m_timercb );
}

void SpectatorMovement::compute_pos( void )
{


}

void SpectatorMovement::Init( InertBody* p_attachedbody, dsreal p_scalepos, long p_posperiod, TimeManager& p_timemanager, const dsstring& p_timername )
{
    m_attachedbody = p_attachedbody;

    m_scalepos = p_scalepos;
    m_posperiod = p_posperiod;


    p_timemanager.AddTimer( p_timername, m_posperiod, m_timercb );

    // pour executer compute_pos() des le 1er appel a SpectatorMovement::Compute(); les appels suivants seront fait periodiquement
    // sur appel timer manager a on_timer()
    m_compute = true;

    p_timemanager.SetTimerState( p_timername, true );
}

void SpectatorMovement::Compute( TimeManager& p_timemanager )
{
    if( m_compute )
    {
        compute_pos();
        m_compute = false;
    }

    m_result.Identity();
}

void SpectatorMovement::on_timer( const dsstring& p_timername )
{
    m_compute = true;
}