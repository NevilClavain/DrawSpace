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
m_meshe_import( NULL )
{
    m_panel = new BasicScenePanel( this );

    m_panel->Show();
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
    Destroy();
}

void RenderFrame::SetGlReady( bool p_ready )
{
    m_gl_ready = p_ready;
}

void RenderFrame::on_render_frame( void )
{
}


void RenderFrame::SetMesheImport( DrawSpace::Interface::MesheImport* p_import )
{
    m_meshe_import = p_import;  
}
