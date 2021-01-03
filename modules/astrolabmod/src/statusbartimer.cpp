/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#include "statusbartimer.h"

StatusBarTimer::StatusBarTimer( const dsstring& p_layout, const dsstring& p_widget_id ) :
m_layout( p_layout ),
m_widget_id( p_widget_id )
{
    m_statusbar_timer_cb = _DRAWSPACE_NEW_( TimerCb, TimerCb( this, &StatusBarTimer::on_statusbar_timer ) );
    m_statusbar_timer = _DRAWSPACE_NEW_( DrawSpace::Utils::Timer, DrawSpace::Utils::Timer );
}

StatusBarTimer::~StatusBarTimer( void )
{

}

void StatusBarTimer::Init( DrawSpace::Utils::TimeManager& p_tm )
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_statusbar_timer->SetHandler( m_statusbar_timer_cb );
    m_statusbar_timer->SetPeriod( 3000 );
    p_tm.RegisterTimer( m_statusbar_timer );

    m_statusbar_timer->SetState( false );
}

void StatusBarTimer::Print( const dsstring& p_msg )
{
    m_renderer->GUI_SetWidgetText( m_layout, m_widget_id, p_msg );
    m_statusbar_timer->SetState( true );
}

void StatusBarTimer::on_statusbar_timer( DrawSpace::Utils::Timer* p_timer )
{
    m_renderer->GUI_SetWidgetText( m_layout, m_widget_id, "" );
    m_statusbar_timer->SetState( false );
}
