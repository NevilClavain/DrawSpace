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

            

    /*
    m_objectslist_dialog = new BasicSceneObjectsListDialog( this );

    wxListCtrl* theList = m_objectslist_dialog->GetListCtrl();

    // Add first column       
    wxListItem col0;
    col0.SetId(0);
    col0.SetText( _("Foo") );
    col0.SetWidth(50);
    theList->InsertColumn(0, col0);

    // Add second column
    wxListItem col1;
    col1.SetId(1);
    col1.SetText( _("Name") );
    col1.SetWidth( 200 );
    theList->InsertColumn(1, col1);

    // Add thirs column     
    wxListItem col2;
    col2.SetId(2);
    col2.SetText( _("Comments") );
    theList->InsertColumn(2, col2);

    ////

    for( long i = 0; i < 10; i++ )
    {
        wxListItem item;
        item.SetId( i );
        item.SetText( "prout" );

        theList->InsertItem( item );

        theList->SetItem( i, 1, "this is my name" );
        theList->SetItem( i, 2, "this is a comment" );
    }

    m_objectslist_dialog->Show();

    ////////////////////////////////////////////

    m_objectproperties_dialog = new BasicSceneObjectPropertiesDialog( this );

    wxPropertyGrid* theGrid = m_objectproperties_dialog->GetPropertyGrid();

    theGrid->Append( new wxStringProperty("Label", "Name", "Initial Value") );

    theGrid->Append( new wxIntProperty("IntProperty", wxPG_LABEL, 12345678) );

    theGrid->Append( new wxFloatProperty("FloatProperty", wxPG_LABEL, 12345.678) );

    theGrid->Append( new wxBoolProperty("BoolProperty", wxPG_LABEL, false) );

    theGrid->Append( new wxFileProperty("FileProperty", wxPG_LABEL, wxEmptyString) );

    theGrid->Append( new wxLongStringProperty("LongStringProperty",
                                     wxPG_LABEL,
                                     "This is much longer string than the "
                                     "first one. Edit it by clicking the button."));

    m_objectproperties_dialog->Show();
    */
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

void RenderFrame::UpdateAll( void )
{
    m_panel->Update();
}
