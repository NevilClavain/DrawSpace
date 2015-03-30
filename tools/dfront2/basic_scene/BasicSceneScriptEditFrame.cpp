/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

BasicSceneScriptEditFrame::BasicSceneScriptEditFrame( wxWindow* p_parent, const dsstring& p_title, const dsstring& p_text )
:ScriptEditFrame( p_parent, wxID_ANY, p_title.c_str() ),
m_console_font( 8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false )
{
    m_script_textCtrl->SetFont( m_console_font );
    m_script_textCtrl->SetValue( p_text );
}

void BasicSceneScriptEditFrame::OnCloseButtonClicked( wxCommandEvent& event )
{
}

void BasicSceneScriptEditFrame::OnApplyButtonClicked( wxCommandEvent& event )
{    
}
