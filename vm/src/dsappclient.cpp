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


dsAppClient::dsAppClient( void ) :
m_bootservice( NULL )
{    
    _INIT_LOGGER( "app.conf" )
    m_w_title = "DrawSpace VM";
}

dsAppClient::~dsAppClient( void )
{
}

void dsAppClient::OnRenderFrame( void )
{
    if( m_bootservice )
    {
        m_bootservice->Run();
    }
}

bool dsAppClient::OnIdleAppInit( void )
{
    Root* root = DrawSpace::Core::SingletonPlugin<Root>::GetInstance()->m_interface;

    m_bootservice = root->InstanciateService( "bootstrap" );

    if( m_bootservice )
    {
        m_bootservice->Init();
        return true;
    }
    else
    {
        return false;
    }
}

void dsAppClient::OnAppInit( void )
{

}

void dsAppClient::OnClose( void )
{
    if( m_bootservice )
    {
        m_bootservice->Release();
    }
}

void dsAppClient::OnKeyPress( long p_key ) 
{

}

void dsAppClient::OnEndKeyPress( long p_key )
{

}

void dsAppClient::OnKeyPulse( long p_key )
{

}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{

}

void dsAppClient::OnMouseLeftButtonDown( long p_xm, long p_ym )
{

}

void dsAppClient::OnMouseLeftButtonUp( long p_xm, long p_ym )
{

}

void dsAppClient::OnMouseRightButtonDown( long p_xm, long p_ym )
{

}

void dsAppClient::OnMouseRightButtonUp( long p_xm, long p_ym )
{

}

void dsAppClient::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{

}
