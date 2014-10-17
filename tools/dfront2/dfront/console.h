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

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <wx/wx.h>
#include "drawspace.h"

class ConsoleDialog : public wxFrame
{
protected:
	DECLARE_EVENT_TABLE()

    wxBoxSizer*                             m_topsizer;
    wxBoxSizer*                             m_buttonssizer;
    wxBoxSizer*                             m_buttonssizer2;

    wxTextCtrl*                             m_textCtrl;
    
    wxButton*                               m_sendcmdButton;
    wxComboBox*				                m_exeCombo;

    wxButton*                               m_insertassetinstanceButton;
    wxComboBox*				                m_assetresourceCombo;
    
    wxButton*                               m_insertconfiginstanceButton;
    wxButton*                               m_insertconfigButton;
    wxComboBox*				                m_configresourceCombo;

    wxFont                                  m_consoleFont;
    wxTextAttr                              m_default_style;

    dsstring                                m_output_text;

    std::map<dsstring, dsstring>            m_exeplugins;

    wxString                                m_title_part1;
    wxString                                m_title_part2;
    bool                                    m_has_path;


    void load_textfile( char* p_path );
    void save_textfile( char* p_path );
    bool save( void );
    bool check_save( void );

    void close( void );
    void update_title( void );
       
public:

    ConsoleDialog( wxWindow* p_parent, const wxString& p_title, const std::map<dsstring, dsstring>& p_exeplugins );
    ~ConsoleDialog( void );

	void OnIdle( wxIdleEvent& p_event );
	void OnClose( wxCloseEvent& p_event );
    void OnButtonSendCmd( wxCommandEvent& p_event );
    void OnButtonInsertAssetInstance( wxCommandEvent& p_event );
    void OnButtonInsertConfigInstance( wxCommandEvent& p_event );
    void OnButtonInsertConfig( wxCommandEvent& p_event );

    void OnMenuNew( wxCommandEvent& p_event );
    void OnMenuOpen( wxCommandEvent& p_event );
    void OnMenuSave( wxCommandEvent& p_event );
    void OnMenuSaveAs( wxCommandEvent& p_event );
    void OnMenuAbout( wxCommandEvent& p_event );
    void OnMenuQuit( wxCommandEvent& p_event );
};


#endif
