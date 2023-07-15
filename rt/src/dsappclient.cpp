/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#include "dsappclient.h"

#include "serviceaspect.h"
#include "serviceaspectimpl.h"
#include "module_root.h"
#include "plugin.h"

#include "parser.h"

dsAppClient* dsAppClient::m_instance = NULL;

using namespace DrawSpace::Interface::Module;
using namespace DrawSpace::Aspect;

static DrawSpace::Logger::Sink logger("vmapp", DrawSpace::Logger::Configuration::getInstance());

dsAppClient::dsAppClient( void ) :
m_mousecursor_visible( true ),
m_service( NULL )
{    
    // init logger
    DrawSpace::Parser::run("logrt.conf", " ", DrawSpace::Logger::Configuration::getInstance()->getParserCallback());

    m_mouse_visible_cb = _DRAWSPACE_NEW_( MouseVisibleCallback, MouseVisibleCallback( this, &dsAppClient::on_mouse_visible ) );
    m_mouse_circularmode_update_cb = _DRAWSPACE_NEW_( MouseCircularModeupdateCallback, MouseCircularModeupdateCallback( this, &dsAppClient::on_mousecircularmode_update ) );
    m_close_app_cb = _DRAWSPACE_NEW_( CloseAppCallback, CloseAppCallback( this, &dsAppClient::on_closeapp ) );
}

dsAppClient::~dsAppClient( void )
{
}

void dsAppClient::OnRenderFrame( void )
{
    m_serviceSystem.Run( &m_entitygraph );
}

bool dsAppClient::OnIdleAppInit( void )
{
    _DSDEBUG(logger, dsstring("RT startup..."))

    Root* root = DrawSpace::Core::SingletonPlugin<Root>::GetInstance()->m_interface;

    m_service = root->InstanciateServiceAspectImpl( "main_app_service" );

    if( m_service )
    {
        dsstring win_title = "DrawSpace Runtime - ";
        
        win_title += root->GetModuleDescr();
        ::SetWindowText( m_hwnd, win_title.c_str() ); 

        // construction du graph avec une entité :)
        ServiceAspect* service_aspect = m_rootEntity.AddAspect<ServiceAspect>();

        // quelques parametres a passer au service...
        service_aspect->AddComponent<DrawSpace::Logger::Configuration*>("logconf", DrawSpace::Logger::Configuration::getInstance());
        service_aspect->AddComponent<DrawSpace::Core::BaseCallback<void, bool>*>("mousecircularmode_cb", m_mouse_circularmode_update_cb);
        service_aspect->AddComponent<DrawSpace::Core::BaseCallback<void, bool>*>("mousevisible_cb", m_mouse_visible_cb);
        service_aspect->AddComponent<DrawSpace::Core::BaseCallback<void, int>*>("closeapp_cb", m_close_app_cb);

        service_aspect->AddImplementation( m_service );

        m_rootEntityNode = m_entitygraph.SetRoot( &m_rootEntity );    
    }
    else
    {
        _DSWARN( logger, dsstring( "no main_app_service found, leaving..." ) )
        return false;
    }
    return true;
}

void dsAppClient::OnAppInit( void )
{    
}

void dsAppClient::OnClose( void )
{
    _DSDEBUG(logger, dsstring("RT shutdown..."))

    //m_serviceSystem.Release( &m_entitygraph );

    ServiceAspect* service_aspect = m_rootEntity.GetAspect<ServiceAspect>();
    service_aspect->RemoveImplementation(m_service);
}

void dsAppClient::OnKeyPress( long p_key ) 
{
    m_serviceSystem.OnKeyPress( &m_entitygraph, p_key );
}

void dsAppClient::OnEndKeyPress( long p_key )
{
    m_serviceSystem.OnEndKeyPress( &m_entitygraph, p_key );
}

void dsAppClient::OnKeyPulse( long p_key )
{
    m_serviceSystem.OnKeyPulse( &m_entitygraph, p_key );
}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    m_serviceSystem.OnMouseMove( &m_entitygraph, p_xm, p_ym, p_dx, p_dy );
}

void dsAppClient::OnMouseWheel( long p_delta )
{
    m_serviceSystem.OnMouseWheel( &m_entitygraph, p_delta );
}

void dsAppClient::OnChar( long p_char, long p_scan )
{
    m_serviceSystem.OnChar( &m_entitygraph, p_char, p_scan );
}

void dsAppClient::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    m_serviceSystem.OnMouseLeftButtonDown( &m_entitygraph, p_xm, p_ym );
}

void dsAppClient::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    m_serviceSystem.OnMouseLeftButtonUp( &m_entitygraph, p_xm, p_ym );
}

void dsAppClient::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    m_serviceSystem.OnMouseRightButtonDown( &m_entitygraph, p_xm, p_ym );
}

void dsAppClient::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    m_serviceSystem.OnMouseRightButtonUp( &m_entitygraph, p_xm, p_ym );
}

void dsAppClient::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
    m_serviceSystem.OnAppEvent( &m_entitygraph, p_wParam, p_lParam );
}

void dsAppClient::on_mouse_visible( bool p_state )
{
    // ne pas appeler ::ShowCursor() quand c'est inutile (par ex. ShowCursor( true ) si le curseur est deja visible), car sinon
    // ca fout le bordel (gestion d'un compteur interne, dixit la doc windows !!!!)

    if( !p_state )
    {
        if( m_mousecursor_visible )
        {
            ::ShowCursor( false );
            m_mousecursor_visible = false;
        }
    }
    else
    {
        if( !m_mousecursor_visible )
        {
            ::ShowCursor( true );
            m_mousecursor_visible = true;
        }
    }
}

void dsAppClient::on_mousecircularmode_update( bool p_state )
{
    m_mouse_circularmode = p_state;

    /*
    // ne pas appeler ::ShowCursor() quand c'est inutile (par ex. ShowCursor( true ) si le curseur est deja visible), car sinon
    // ca fout le bordel (gestion d'un compteur interne, dixit la doc windows !!!!)

    if( p_state )
    {
        if( m_mousecursor_visible )
        {
            ::ShowCursor( false );
            m_mousecursor_visible = false;
        }
    }
    else
    {
        if( !m_mousecursor_visible )
        {
            ::ShowCursor( true );
            m_mousecursor_visible = true;
        }
    }
    */
}

void dsAppClient::on_closeapp( int p_code )
{
    Quit( p_code );
}
