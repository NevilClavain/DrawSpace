/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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


#include "servicesystem.h"
#include "serviceaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Systems;

ServiceSystem::ServiceSystem(void)
{
}

ServiceSystem::~ServiceSystem(void)
{
}

bool ServiceSystem::init( EntityGraph::EntityNodeGraph* p_entitygraph )
{
    m_init_status = true;
    m_callsource = "Init";
    p_entitygraph->AcceptSystemLeafToRoot( this );

    return m_init_status;
}

void ServiceSystem::release( EntityGraph::EntityNodeGraph* p_entitygraph )
{
    m_callsource = "Release";
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::run( EntityNodeGraph* p_entitygraph )
{
    m_callsource = "Run";
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::VisitEntity( Entity* p_parent, Entity* p_entity )
{
    ServiceAspect* service_aspect = p_entity->GetAspect<ServiceAspect>();
    if( service_aspect )
    {
        if( "Init" == m_callsource )
        {
            if( false == service_aspect->Init() )
            {
                m_init_status = false;
            }        
        }
        else if( "Run" == m_callsource )
        {
            service_aspect->Run();
        }
        else if( "Release" == m_callsource )
        {
            service_aspect->Release();
        }
        else if( "OnKeyPress" == m_callsource )
        {
            service_aspect->OnKeyPress( m_key );
        }
        else if( "OnEndKeyPress" == m_callsource )
        {
            service_aspect->OnEndKeyPress( m_key );
        }
        else if( "OnKeyPulse" == m_callsource )
        {
            service_aspect->OnKeyPulse( m_key );
        }
        else if( "OnChar" == m_callsource )
        {
            service_aspect->OnChar( m_char, m_scan );
        }
        else if( "OnMouseMove" == m_callsource )
        {
            service_aspect->OnMouseMove( m_xm, m_ym, m_dx, m_dy );
        }
        else if( "OnMouseWheel" == m_callsource )
        {
            service_aspect->OnMouseWheel( m_delta );
        }
        else if( "OnMouseLeftButtonDown" == m_callsource )
        {
            service_aspect->OnMouseLeftButtonDown( m_xm, m_ym );
        }
        else if( "OnMouseLeftButtonUp" == m_callsource )
        {
            service_aspect->OnMouseLeftButtonUp( m_xm, m_ym );
        }
        else if( "OnMouseRightButtonDown" == m_callsource )
        {
            service_aspect->OnMouseRightButtonDown( m_xm, m_ym );
        }
        else if( "OnMouseRightButtonUp" == m_callsource )
        {
            service_aspect->OnMouseRightButtonUp( m_xm, m_ym );
        }
        else if( "OnAppEvent" == m_callsource )
        {
            service_aspect->OnAppEvent( m_wParam, m_lParam );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ServiceSystem::OnKeyPress( EntityGraph::EntityNodeGraph* p_entitygraph, long p_key )
{
    m_callsource = "OnKeyPress";
    m_key = p_key;
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::OnEndKeyPress( EntityGraph::EntityNodeGraph* p_entitygraph, long p_key )
{
    m_callsource = "OnEndKeyPress";
    m_key = p_key;
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::OnKeyPulse( EntityGraph::EntityNodeGraph* p_entitygraph, long p_key )
{
    m_callsource = "OnKeyPulse";
    m_key = p_key;
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::OnChar( EntityGraph::EntityNodeGraph* p_entitygraph, long p_char, long p_scan )
{
    m_callsource = "OnChar";
    m_char = p_char;
    m_scan = p_scan;
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::OnMouseMove( EntityGraph::EntityNodeGraph* p_entitygraph, long p_xm, long p_ym, long p_dx, long p_dy )
{
    m_callsource = "OnMouseMove";
    m_xm = p_xm;
    m_ym = p_ym;
    m_dx = p_dx;
    m_dy = p_dy;
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::OnMouseWheel( EntityGraph::EntityNodeGraph* p_entitygraph, long p_delta )
{
    m_callsource = "OnMouseWheel";
    m_delta = p_delta;
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::OnMouseLeftButtonDown( EntityGraph::EntityNodeGraph* p_entitygraph, long p_xm, long p_ym )
{
    m_callsource = "OnMouseLeftButtonDown";
    m_xm = p_xm;
    m_ym = p_ym;
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::OnMouseLeftButtonUp( EntityGraph::EntityNodeGraph* p_entitygraph, long p_xm, long p_ym )
{
    m_callsource = "OnMouseLeftButtonUp";
    m_xm = p_xm;
    m_ym = p_ym;
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::OnMouseRightButtonDown( EntityGraph::EntityNodeGraph* p_entitygraph, long p_xm, long p_ym )
{
    m_callsource = "OnMouseRightButtonDown";
    m_xm = p_xm;
    m_ym = p_ym;
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::OnMouseRightButtonUp( EntityGraph::EntityNodeGraph* p_entitygraph, long p_xm, long p_ym )
{
    m_callsource = "OnMouseRightButtonUp";
    m_xm = p_xm;
    m_ym = p_ym;
    p_entitygraph->AcceptSystemLeafToRoot( this );
}

void ServiceSystem::OnAppEvent( EntityGraph::EntityNodeGraph* p_entitygraph, WPARAM p_wParam, LPARAM p_lParam )
{
    m_callsource = "OnAppEvent";
    m_wParam = p_wParam;
    m_lParam = p_lParam;
    p_entitygraph->AcceptSystemLeafToRoot( this );
}
