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

#include "console.h"

BEGIN_EVENT_TABLE( ConsoleDialog, wxFrame )
    EVT_IDLE( ConsoleDialog::OnIdle )
	EVT_CLOSE( ConsoleDialog::OnClose )
END_EVENT_TABLE()


ConsoleDialog::ConsoleDialog( wxWindow* p_parent, const wxString& p_title ) :
wxFrame( p_parent, wxID_ANY, p_title, wxPoint( 50, 660 ), wxSize( 800, 200 ), wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLIP_CHILDREN ),
m_consoleFont( 10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false )
{
    m_topsizer = new wxBoxSizer( wxVERTICAL );
    m_textCtrl = new wxTextCtrl( this, -1, ">", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
        
    m_textCtrl->SetFont( m_consoleFont );
    

    m_topsizer->Add(

        m_textCtrl,                       
        1,            // make vertically stretchable
        wxEXPAND |    // make horizontally stretchable
        wxALL,        //   and make border all around
        10 );         // set border width to 10

    SetSizer( m_topsizer );    
}

ConsoleDialog::~ConsoleDialog( void )
{    
}

void ConsoleDialog::OnIdle( wxIdleEvent& p_event )
{

}

void ConsoleDialog::OnClose( wxCloseEvent& p_event )
{
}
