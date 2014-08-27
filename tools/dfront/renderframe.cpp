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


#include "renderframe.h"
#include "resources.h"

#include "scene.h"

RenderFrame*    RenderFrame::m_instance = NULL;

wxString        RenderFrame::m_caption;
wxSize          RenderFrame::m_size;

BEGIN_EVENT_TABLE( RenderFrame, wxFrame )
    EVT_IDLE( RenderFrame::OnIdle )
	EVT_CLOSE( RenderFrame::OnClose )
END_EVENT_TABLE()

RenderFrame::RenderFrame( void ) : 
wxFrame( NULL, wxID_ANY, m_caption, wxPoint( 50, 50 ), m_size, wxMINIMIZE_BOX | wxCLOSE_BOX | wxSYSTEM_MENU | wxCAPTION ),
m_gl_ready( false ),
m_current_rendered_scene( NULL )
{

    m_dialog = _DRAWSPACE_NEW_( Dialog, Dialog( this, _( "Tools" ), this ) );
    m_dialog->Show();

    m_console_dialog = _DRAWSPACE_NEW_( ConsoleDialog, ConsoleDialog( this, _( "User inputs" ) ) );
    m_console_dialog->Show();
}


RenderFrame::~RenderFrame( void )
{
}

void RenderFrame::OnIdle( wxIdleEvent& p_event )
{
    p_event.RequestMore( true );
	if( m_gl_ready )
    {
        on_render_frame();
    }
}

void RenderFrame::OnClose( wxCloseEvent& p_event )
{
    m_dialog->Close();
    m_console_dialog->Close();
    Destroy();
}

void RenderFrame::SetGlReady( bool p_ready )
{
    m_gl_ready = p_ready;
}

void RenderFrame::on_render_frame( void )
{
    if( m_current_rendered_scene )
    {
        m_current_rendered_scene->Draw();
    }
}

void RenderFrame::SetLuaContext( DrawSpace::LuaContext* p_luacontext )
{
    m_console_dialog->SetLuaContext( p_luacontext );
    m_luacontext = p_luacontext;
}

bool RenderFrame::SetCurrentScene( Scene* p_scene )
{
    dsstring scene_name;
    p_scene->GetName( scene_name );

    if( scene_name == "" )
    {
        return false;
    }
    m_current_rendered_scene = p_scene;
    return true;
}

void RenderFrame::PrintConsole( const dsstring& p_text )
{
    m_console_dialog->Print( p_text );
}