/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#include "BasicSceneScriptEditFrame.h"
#include <wx/filedlg.h>

BasicSceneScriptEditFrame::BasicSceneScriptEditFrame( wxWindow* p_parent, const dsstring& p_title, dsstring* p_text, bool* p_script_state )
:ScriptEditFrame( p_parent, wxID_ANY, p_title.c_str() ),
m_console_font( 8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false ),
m_text( p_text ),
m_script_state( p_script_state )
{
    m_script_textCtrl->SetFont( m_console_font );
    //m_script_textCtrl->SetValue( p_text->c_str() );
    //m_activation_checkBox->SetValue( *m_script_state );
}

void BasicSceneScriptEditFrame::OnCloseButtonClicked( wxCommandEvent& event )
{
    *m_script_state = m_activation_checkBox->GetValue();
    //Close();
    ScriptEditFrame::Show( false );
}

void BasicSceneScriptEditFrame::OnApplyButtonClicked( wxCommandEvent& event )
{
    wxString newtext = m_script_textCtrl->GetValue();

    *m_text = newtext.data();
    *m_script_state = m_activation_checkBox->GetValue();    
}

void BasicSceneScriptEditFrame::OnLoadButtonClicked( wxCommandEvent& event )
{
    wxFileDialog openFileDialog( this, "Open lua script", "", "", "lua files (*.lua)|*.lua", wxFD_OPEN | wxFD_FILE_MUST_EXIST );
    if( wxID_CANCEL == openFileDialog.ShowModal() )
    {
        return;
    }
    wxString path = openFileDialog.GetPath();
    m_script_textCtrl->LoadFile( path );
}

void BasicSceneScriptEditFrame::OnClearButtonClicked( wxCommandEvent& event )
{
    m_script_textCtrl->Clear();
}

void BasicSceneScriptEditFrame::Show( void )
{
    m_script_textCtrl->SetValue( m_text->c_str() );
    m_activation_checkBox->SetValue( *m_script_state );
    ScriptEditFrame::Show();    
}

dsstring* BasicSceneScriptEditFrame::GetTextRef( void )
{
    return m_text;
}

bool* BasicSceneScriptEditFrame::GeScriptStateRef( void )
{
    return m_script_state;
}
