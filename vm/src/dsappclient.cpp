/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#include "dsappclient.h"

dsAppClient* dsAppClient::m_instance = NULL;

using namespace DrawSpace::Interface::Module;


_DECLARE_DS_LOGGER( logger, "vmapp", DrawSpace::Logger::Configuration::GetInstance() )


dsAppClient::dsAppClient( void ) :
m_mainloopservice( NULL ),
m_mousecursor_visible( true )
{    
    _INIT_LOGGER( "logvm.conf" )
    m_w_title = "DrawSpace VM";

    m_mouse_visible_cb = _DRAWSPACE_NEW_( MouseVisibleCallback, MouseVisibleCallback( this, &dsAppClient::on_mouse_visible ) );
    m_mouse_circularmode_update_cb = _DRAWSPACE_NEW_( MouseCircularModeupdateCallback, MouseCircularModeupdateCallback( this, &dsAppClient::on_mousecircularmode_update ) );
    m_close_app_cb = _DRAWSPACE_NEW_( CloseAppCallback, CloseAppCallback( this, &dsAppClient::on_closeapp ) );
}

dsAppClient::~dsAppClient( void )
{
}

void dsAppClient::OnRenderFrame( void )
{
    if( m_mainloopservice )
    {
        m_mainloopservice->Run();
    }
}

bool dsAppClient::OnIdleAppInit( void )
{
    _DSDEBUG(logger, dsstring("VM startup..."))

    Root* root = DrawSpace::Core::SingletonPlugin<Root>::GetInstance()->m_interface;

    m_mainloopservice = root->InstanciateService( "mainloop" );
    if( m_mainloopservice )
    {
        dsstring win_title = "DrawSpace VM - ";
        
        win_title += root->GetModuleDescr();
        ::SetWindowText( m_hwnd, win_title.c_str() );

        _DSDEBUG(logger, dsstring("mainloop service initialisation"))
        m_mainloopservice->Init( DrawSpace::Logger::Configuration::GetInstance(), m_mouse_circularmode_update_cb, m_mouse_visible_cb, m_close_app_cb );

        return true;
    }
    else
    {
        _DSWARN(logger, dsstring("no mainloop service found, leaving..."))
        return false;
    }
}

void dsAppClient::OnAppInit( void )
{    
}

void dsAppClient::OnClose( void )
{
    _DSDEBUG(logger, dsstring("VM shutdown..."))
    if( m_mainloopservice )
    {
        _DSDEBUG(logger, dsstring("closing main loop service..."))
        m_mainloopservice->Release();
    }
}

void dsAppClient::OnKeyPress( long p_key ) 
{
    if( m_mainloopservice )
    {
        m_mainloopservice->OnKeyPress( p_key );
    }
}

void dsAppClient::OnEndKeyPress( long p_key )
{
    if( m_mainloopservice )
    {
        m_mainloopservice->OnEndKeyPress( p_key );
    }
}

void dsAppClient::OnKeyPulse( long p_key )
{
    if( m_mainloopservice )
    {
        m_mainloopservice->OnKeyPulse( p_key );
    }
}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    if( m_mainloopservice )
    {
        m_mainloopservice->OnMouseMove( p_xm, p_ym, p_dx, p_dy );
    }
}

void dsAppClient::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    if( m_mainloopservice )
    {
        m_mainloopservice->OnMouseLeftButtonDown( p_xm, p_ym );
    }
}

void dsAppClient::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    if( m_mainloopservice )
    {
        m_mainloopservice->OnMouseLeftButtonUp( p_xm, p_ym );
    }
}

void dsAppClient::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    if( m_mainloopservice )
    {
        m_mainloopservice->OnMouseRightButtonDown( p_xm, p_ym );
    }
}

void dsAppClient::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    if( m_mainloopservice )
    {
        m_mainloopservice->OnMouseRightButtonUp( p_xm, p_ym );
    }
}

void dsAppClient::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
    if( m_mainloopservice )
    {
        m_mainloopservice->OnAppEvent( p_wParam, p_lParam );
    }
}

void dsAppClient::on_mouse_visible( bool p_state )
{
    // ne pas appeler ::ShowCursor() quand c'est inutile (par ex. ShowCursor( true ) si le curseur est déja visible), car sinon
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
    // ne pas appeler ::ShowCursor() quand c'est inutile (par ex. ShowCursor( true ) si le curseur est déja visible), car sinon
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