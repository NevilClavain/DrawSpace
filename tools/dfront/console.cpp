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
#include "resources.h"

BEGIN_EVENT_TABLE( ConsoleDialog, wxFrame )
    EVT_IDLE( ConsoleDialog::OnIdle )
	EVT_CLOSE( ConsoleDialog::OnClose )
    EVT_BUTTON( wxID_SENDCMDBUTTON, ConsoleDialog::OnButtonSendCmd )
    EVT_BUTTON( wxID_CLEARCMDBUTTON, ConsoleDialog::OnButtonClearCmd )
END_EVENT_TABLE()


ConsoleDialog::ConsoleDialog( wxWindow* p_parent, const wxString& p_title ) :
wxFrame( p_parent, wxID_ANY, p_title, wxPoint( 50, 660 ), wxSize( 800, 250 ), wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLIP_CHILDREN ),
m_consoleFont( 10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false )
{
    m_topsizer = new wxBoxSizer( wxVERTICAL );

    m_buttonssizer = new wxBoxSizer( wxHORIZONTAL );

    m_textCtrl = new wxTextCtrl( this, wxID_CONSOLETEXTCTRL, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );        
    m_textCtrl->SetFont( m_consoleFont );

    m_textoutputsCtrl = new wxTextCtrl( this, wxID_CONSOLETEXTCTRL, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );        
    m_textoutputsCtrl->SetFont( m_consoleFont );

    m_sendcmdButton = new wxButton( this, wxID_SENDCMDBUTTON, "Send command" );
    m_clearcmdButton = new wxButton( this, wxID_CLEARCMDBUTTON, "Clear command" );


    m_topsizer->Add(

        m_textCtrl,                       
        1,            // make vertically stretchable
        wxEXPAND |    // make horizontally stretchable
        wxALL,        //   and make border all around
        10 );         // set border width to 10

    m_buttonssizer->Add(

        m_sendcmdButton,                       
        0,            // make NOT vertically stretchable
        0,   // make NOT horizontally stretchable        
        0 );         // set border width to 0

    m_buttonssizer->Add(

        m_clearcmdButton,                       
        0,            // make NOT vertically stretchable
        0,   // make NOT horizontally stretchable        
        0 );         // set border width to 0


    m_topsizer->Add(

        m_buttonssizer,                       
        0,            // make NOT vertically stretchable
        0,          // make NOT horizontally stretchable   
        10 );         // set border width to 10

    m_topsizer->Add(

        m_textoutputsCtrl,                       
        0,            // make NOT vertically stretchable
        wxEXPAND |    // make horizontally stretchable
        wxALL,        //   and make border all around
        10 );         // set border width to 10

    SetSizer( m_topsizer );


    m_luaerrorhandler = new LuaErrorHandler( this, &ConsoleDialog::on_luaerror );
}

ConsoleDialog::~ConsoleDialog( void )
{    
}

void ConsoleDialog::on_luaerror( const dsstring& p_errstr )
{
    /*
    m_textoutputsCtrl->Clear();
    m_textoutputsCtrl->SetValue( wxString( p_errstr.c_str() ) );
    */

    Print( p_errstr );
}

void ConsoleDialog::OnIdle( wxIdleEvent& p_event )
{
}

void ConsoleDialog::OnClose( wxCloseEvent& p_event )
{
}

void ConsoleDialog::OnButtonSendCmd( wxCommandEvent& p_event )
{
    Print( "Command submitted" );
    m_luacontext->Exec( m_textCtrl->GetValue().c_str() );
}

void ConsoleDialog::OnButtonClearCmd( wxCommandEvent& p_event )
{
    int answer = wxMessageBox( "Clear text?", "Confirm", wxYES_NO );
    if( wxYES == answer )
    {
        m_textCtrl->Clear();
    }
}

void ConsoleDialog::SetLuaContext( DrawSpace::LuaContext* p_luacontext )
{
    m_luacontext = p_luacontext;
    m_luacontext->RegisterErrorHandler( m_luaerrorhandler );
}

void ConsoleDialog::Print( const dsstring& p_text )
{
    //m_textoutputsCtrl->SetValue( wxString( p_text.c_str() ) );

    m_output_text = m_output_text + p_text;
    m_output_text = m_output_text + dsstring( "\n" );

    //m_textoutputsCtrl->SetValue( wxString( m_output_text.c_str() ) );

    m_textoutputsCtrl->AppendText( wxString( p_text.c_str() ) );
    m_textoutputsCtrl->AppendText( wxString( "\n" ) );

    
}