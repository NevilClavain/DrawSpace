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
#include "file.h"
#include <wx/clipbrd.h>


BEGIN_EVENT_TABLE( ConsoleDialog, wxFrame )

    EVT_MENU( wxID_MENUNEW,   ConsoleDialog::OnMenuNew )
    EVT_MENU( wxID_MENUOPEN,   ConsoleDialog::OnMenuOpen )
    EVT_MENU( wxID_MENUSAVE,   ConsoleDialog::OnMenuSave )
    EVT_MENU( wxID_MENUSAVEAS,   ConsoleDialog::OnMenuSaveAs )
    EVT_MENU( wxID_EXIT,  ConsoleDialog::OnMenuQuit )
    EVT_MENU( wxID_ABOUT, ConsoleDialog::OnMenuAbout )

    EVT_IDLE( ConsoleDialog::OnIdle )
	EVT_CLOSE( ConsoleDialog::OnClose )
    EVT_BUTTON( wxID_SENDCMDBUTTON, ConsoleDialog::OnButtonSendCmd )
    EVT_BUTTON( wxID_INSERTASSETINSTANCEBUTTON, ConsoleDialog::OnButtonInsertAssetInstance )
    EVT_BUTTON( wxID_INSERTCONFIGINSTANCEBUTTON, ConsoleDialog::OnButtonInsertConfigInstance )
    EVT_BUTTON( wxID_INSERTCONFIGBUTTON, ConsoleDialog::OnButtonInsertConfig )
END_EVENT_TABLE()


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


ConsoleDialog::ConsoleDialog( wxWindow* p_parent, const wxString& p_title, const std::map<dsstring, dsstring>& p_exeplugins ) :
wxFrame( p_parent, wxID_ANY, p_title, wxPoint( 50, 60 ), wxSize( 800, 650 ), wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN ),
m_consoleFont( 10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false ),
m_exeplugins( p_exeplugins ),
m_title_part1( p_title ),
m_has_path( false )
{
    ////// Menu bar build

    wxMenu *menuFile = new wxMenu;
    
    menuFile->Append( wxID_MENUNEW, "&New...",
                     "New text file" );
    
    menuFile->Append( wxID_MENUOPEN, "&Open...",
                     "Open text file" );


    menuFile->AppendSeparator();

    menuFile->Append( wxID_MENUSAVE, "&Save...",
                     "Save text file" );


    menuFile->Append( wxID_MENUSAVEAS, "&Save as...",
                     "Save text file" );

    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, "&File" );
    menuBar->Append( menuHelp, "&Help" );
    SetMenuBar( menuBar );


    ////// Window content build

    m_topsizer = new wxBoxSizer( wxVERTICAL );

    m_buttonssizer = new wxBoxSizer( wxHORIZONTAL );
    m_buttonssizer2 = new wxBoxSizer( wxHORIZONTAL );

    m_textCtrl = new wxTextCtrl( this, wxID_CONSOLETEXTCTRL, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    m_textCtrl->SetFont( m_consoleFont );

    m_sendcmdButton = new wxButton( this, wxID_SENDCMDBUTTON, "Launch" );

    m_exeCombo = new wxComboBox( this, wxID_EXECOMBO, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );

    for( std::map<dsstring, dsstring>::iterator it = m_exeplugins.begin(); it != m_exeplugins.end(); ++it )
    {
        m_exeCombo->Append( wxString( it->first ) );
    }
    m_exeCombo->SetSelection( 0 );


    m_insertassetinstanceButton = new wxButton( this, wxID_INSERTASSETINSTANCEBUTTON, "Insert asset instance" );
	m_assetresourceCombo = new wxComboBox( this, wxID_ASSETCOMBO, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );

    m_insertconfiginstanceButton = new wxButton( this, wxID_INSERTCONFIGINSTANCEBUTTON, "Insert config instance" );
    m_insertconfigButton = new wxButton( this, wxID_INSERTCONFIGBUTTON, "Insert config" );
	m_configresourceCombo = new wxComboBox( this, wxID_CONFIGCOMBO, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );


    std::vector<dsstring> assets_list;
    std::vector<dsstring> config_list;

    DrawSpace::Core::Factory* factory = DrawSpace::Core::Factory::GetInstance();

    factory->GetAssetKeywordsList( assets_list );
    factory->GetConfigKeywordsList( config_list );

    for( size_t i = 0; i < assets_list.size(); i++ )
    {
        m_assetresourceCombo->Append( wxString( assets_list[i].c_str() ) );
    }
    for( size_t i = 0; i < config_list.size(); i++ )
    {
        m_configresourceCombo->Append( wxString( config_list[i].c_str() ) );
    }

    m_assetresourceCombo->SetSelection( 0 );
    m_configresourceCombo->SetSelection( 0 );


    
    m_topsizer->Add(

        m_textCtrl,                       
        1,            // make vertically stretchable
        wxEXPAND |    // make horizontally stretchable
        wxALL,        //   and make border all around
        0 );         // set border width to 0


    m_buttonssizer->Add(

        m_exeCombo,                       
        0,            // make NOT vertically stretchable
        0,   // make NOT horizontally stretchable        
        0 );         // set border width to 0


    m_buttonssizer->Add(

        m_sendcmdButton,                       
        0,            // make NOT vertically stretchable
        0,   // make NOT horizontally stretchable        
        0 );         // set border width to 0


    m_buttonssizer->Add(

        m_insertassetinstanceButton,                       
        0,            // make NOT vertically stretchable
        0,   // make NOT horizontally stretchable        
        0 );         // set border width to 0


    m_buttonssizer->Add(

        m_assetresourceCombo,                       
        0,            // make NOT vertically stretchable
        0,   // make NOT horizontally stretchable        
        0 );         // set border width to 0


    m_buttonssizer->Add(

        m_insertconfiginstanceButton,                       
        0,            // make NOT vertically stretchable
        0,   // make NOT horizontally stretchable        
        0 );         // set border width to 0


    m_buttonssizer->Add(

        m_insertconfigButton,                       
        0,            // make NOT vertically stretchable
        0,   // make NOT horizontally stretchable        
        0 );         // set border width to 0


    m_buttonssizer->Add(

        m_configresourceCombo,                       
        0,            // make NOT vertically stretchable
        0,   // make NOT horizontally stretchable        
        0 );         // set border width to 0


    m_topsizer->Add(

        m_buttonssizer,                       
        0,            // make NOT vertically stretchable
        0,          // make NOT horizontally stretchable   
        0 );         // set border width to 0



    SetSizer( m_topsizer );

    update_title();
}

ConsoleDialog::~ConsoleDialog( void )
{    
}

void ConsoleDialog::update_title( void )
{
    SetTitle( m_title_part1 + wxString( " - " ) + m_title_part2 );
}

void ConsoleDialog::OnIdle( wxIdleEvent& p_event )
{
}

void ConsoleDialog::OnClose( wxCloseEvent& p_event )
{
    close();
}

void ConsoleDialog::OnButtonSendCmd( wxCommandEvent& p_event )
{    
}

void ConsoleDialog::OnButtonInsertAssetInstance( wxCommandEvent& p_event )
{
    dsstring selected_keyword = m_assetresourceCombo->GetString( m_assetresourceCombo->GetCurrentSelection() );

    Asset* asset = DrawSpace::Core::Factory::GetInstance()->InstanciateAssetFromKeyword( selected_keyword );

    dsstring text;
    asset->DumpProperties( text );
    dsstring type_name;
    asset->GetKeyword( type_name );

    text = dsstring( DECLARE_ASSET_INSTANCE_KEYWORD ) + dsstring( " " ) + type_name + dsstring( "\r\n\r\n" ) + text;

    text += dsstring( "\r\n" );
    text += dsstring( DECLARE_END_ASSET_KEYWORD );

    if( wxTheClipboard->Open() )
    {
        wxTheClipboard->SetData( new wxTextDataObject( wxString( text.c_str() ) ) );
        wxTheClipboard->Close();
    }

    m_textCtrl->Paste();
}

void ConsoleDialog::OnButtonInsertConfigInstance( wxCommandEvent& p_event )
{
    dsstring selected_keyword = m_configresourceCombo->GetString( m_configresourceCombo->GetCurrentSelection() );

    Configurable* config = DrawSpace::Core::Factory::GetInstance()->InstanciateConfigFromKeyword( selected_keyword );

    dsstring text;
    config->DumpProperties( text );
    dsstring type_name;
    config->GetKeyword( type_name );

    text = dsstring( DECLARE_CONFIG_INSTANCE_KEYWORD ) + dsstring( " " ) + type_name + dsstring( " <name>\r\n\r\n" ) + text;

    text += dsstring( "\r\n" );
    text += dsstring( DECLARE_END_CONFIG_KEYWORD );

    if( wxTheClipboard->Open() )
    {
        wxTheClipboard->SetData( new wxTextDataObject( wxString( text.c_str() ) ) );
        wxTheClipboard->Close();
    }

    m_textCtrl->Paste();
}

void ConsoleDialog::OnButtonInsertConfig( wxCommandEvent& p_event )
{
    dsstring selected_keyword = m_configresourceCombo->GetString( m_configresourceCombo->GetCurrentSelection() );

    Configurable* config = DrawSpace::Core::Factory::GetInstance()->InstanciateConfigFromKeyword( selected_keyword );

    dsstring text;
    config->DumpProperties( text );
    dsstring type_name;
    config->GetKeyword( type_name );

    text = dsstring( DECLARE_CONFIG_KEYWORD ) + dsstring( " " ) + type_name + dsstring( " <name>\r\n\r\n" ) + text;

    text += dsstring( "\r\n" );
    text += dsstring( DECLARE_END_CONFIG_KEYWORD );

    if( wxTheClipboard->Open() )
    {
        wxTheClipboard->SetData( new wxTextDataObject( wxString( text.c_str() ) ) );
        wxTheClipboard->Close();
    }

    m_textCtrl->Paste();
}

void ConsoleDialog::load_textfile( char* p_path )
{
    /*
    DrawSpace::Utils::File file( p_path, DrawSpace::Utils::File::OPENEXISTING );
    DrawSpace::Utils::Archive arc;

    file.LoadArchive( arc );

    m_textCtrl->Clear();

    char* arc_content = new char[arc.GetTotalLength() + 1];
    memcpy( arc_content, arc.GetCurrentPtr(), arc.GetTotalLength() );

    arc_content[arc.GetTotalLength()] = 0;
    
    wxString content( arc_content );
    m_textCtrl->SetValue( content );
    */

    m_textCtrl->LoadFile( p_path );
    m_has_path = true;
}

void ConsoleDialog::save_textfile( char* p_path )
{
    /*
    wxString text = m_textCtrl->GetValue();

    DrawSpace::Utils::Archive arc;
    arc.SetArchiveTotalLength( (unsigned long)text.Length() );
    memcpy( arc.GetCurrentPtr(), text.c_str(), text.Length() );

    DrawSpace::Utils::File file( p_path, DrawSpace::Utils::File::CREATENEW );
    file.SaveArchive( arc );
    */

    m_textCtrl->SaveFile( p_path ); 
}

bool ConsoleDialog::save( void )
{
    wxString path;
    if( m_has_path )
    {
        int answer = wxMessageBox( "Save text modifications?", "Confirm", wxYES_NO );
        if( wxNO == answer )
        {
            return false;
        }
        path = m_title_part2;
    }
    else
    {
        wxFileDialog saveFileDialog( this, "save resources file", "", "", "txt files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
        if( wxID_CANCEL == saveFileDialog.ShowModal() )
        {
            return false;
        }
        path = saveFileDialog.GetPath();
    }
    save_textfile( (char *)path.c_str() );
    return true;
}

bool ConsoleDialog::check_save( void )
{
    if( m_textCtrl->IsModified() )
    {
        return save();
    }
    return true;
}

void ConsoleDialog::OnMenuNew( wxCommandEvent& p_event )
{
    check_save();
    m_has_path = false;
    m_title_part2 = "";
    update_title();
    m_textCtrl->Clear();
}

void ConsoleDialog::OnMenuOpen( wxCommandEvent& p_event )
{
    check_save();

    wxFileDialog openFileDialog( this, "Open resources file", "", "", "txt files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST );
    if( wxID_CANCEL == openFileDialog.ShowModal() )
    {
        return;
    }
    wxString path = openFileDialog.GetPath();
    load_textfile( (char *)path.c_str() );

    m_title_part2 = path;
    update_title();

}

void ConsoleDialog::OnMenuSave( wxCommandEvent& p_event )
{    
    save();
}

void ConsoleDialog::OnMenuSaveAs( wxCommandEvent& p_event )
{
    wxString path;
    wxFileDialog saveFileDialog( this, "save resources file", "", "", "txt files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
    if( wxID_CANCEL == saveFileDialog.ShowModal() )
    {
        return;
    }
    path = saveFileDialog.GetPath();    
    save_textfile( (char *)path.c_str() );
}

void ConsoleDialog::OnMenuAbout( wxCommandEvent& p_event )
{
    wxMessageBox( "DrawFRONT v1.0", "About", wxOK );
}

void ConsoleDialog::OnMenuQuit( wxCommandEvent& p_event )
{    
    close();
}

void ConsoleDialog::close( void )
{
    check_save();
    Destroy();
}