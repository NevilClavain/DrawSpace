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

#include "dialog.h"
#include "resources.h"
#include "renderframe.h"


BEGIN_EVENT_TABLE( Dialog, wxDialog )
    EVT_IDLE( Dialog::OnIdle )
	EVT_CLOSE( Dialog::OnClose )
END_EVENT_TABLE()


Dialog::Dialog( wxWindow* p_parent, const wxString& p_title, RenderFrame* p_owner ) :
wxDialog( p_parent, wxID_ANY, p_title, wxPoint( 860, 50 ), wxSize( 400, 600 ), wxCAPTION ),
m_owner( p_owner )
{
    m_notebook = new wxNotebook( this, wxID_NOTEBOOK, wxPoint( 0, 0 ), wxSize( 400, 600 ), wxNB_TOP | wxNB_FIXEDWIDTH );

    m_topsizer = new wxBoxSizer( wxVERTICAL );

    ///////////////////////////////////////////////////////////////////////////

	m_scenes_tab = new wxWindow( m_notebook, wxID_TAB_SCENES );

    m_scenes_listbox = new wxListBox( m_scenes_tab, wxID_SCENESLISTBOX, wxPoint( 0, 0 ), wxSize( 200, 200 ) );
    m_newscene_button = new wxButton( m_scenes_tab, wxID_NEWSCENEBUTTON, "New scene", wxPoint( 210, 0 ) );


    m_passes_listbox = new wxListBox( m_scenes_tab, wxID_PASSESLISTBOX, wxPoint( 0, 240 ), wxSize( 200, 200 ) );
    m_newpass_button = new wxButton( m_scenes_tab, wxID_NEWPASSBUTTON, "New pass", wxPoint( 210, 240 ) );


    ///////////////////////////////////////////////////////////////////////////

	m_scenegraph_tab = new wxWindow( m_notebook, wxID_TAB_SCENEGRAPH );


    m_notebook->AddPage( m_scenes_tab, "Scenes", true );
    m_notebook->AddPage( m_scenegraph_tab, "Scenegraph", true );


    m_topsizer->Add( m_notebook, 0, wxALL, 0 );

    SetSizer( m_topsizer );

    m_notebook->SetSelection( 0 );
}

Dialog::~Dialog( void )
{

}

void Dialog::OnIdle( wxIdleEvent& p_event )
{

}

void Dialog::OnClose( wxCloseEvent& p_event )
{
}
