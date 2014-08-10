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

class ConsoleDialog : public wxFrame
{
protected:
	DECLARE_EVENT_TABLE()

    wxBoxSizer*             m_topsizer;

    wxTextCtrl*             m_textCtrl;

    wxFont                  m_consoleFont;
    wxTextAttr              m_default_style;

public:

    ConsoleDialog( wxWindow* p_parent, const wxString& p_title );
    ~ConsoleDialog( void );

	void OnIdle( wxIdleEvent& p_event );
	void OnClose( wxCloseEvent& p_event );

};


#endif