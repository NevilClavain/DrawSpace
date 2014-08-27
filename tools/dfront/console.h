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
#include "lua_context.h"
#include "callback.h"

class ConsoleDialog : public wxFrame
{
protected:
	DECLARE_EVENT_TABLE()

    typedef DrawSpace::Core::CallBack<ConsoleDialog, void, const dsstring&> LuaErrorHandler;

    wxBoxSizer*             m_topsizer;
    wxBoxSizer*             m_buttonssizer;

    wxTextCtrl*             m_textCtrl;
    wxTextCtrl*             m_textoutputsCtrl;
    wxButton*               m_sendcmdButton;
    wxButton*               m_clearcmdButton;

    wxFont                  m_consoleFont;
    wxTextAttr              m_default_style;

    DrawSpace::LuaContext*  m_luacontext;

    LuaErrorHandler*        m_luaerrorhandler;

    dsstring                m_output_text;

    void on_luaerror( const dsstring& p_errstr );

public:

    ConsoleDialog( wxWindow* p_parent, const wxString& p_title );
    ~ConsoleDialog( void );

	void OnIdle( wxIdleEvent& p_event );
	void OnClose( wxCloseEvent& p_event );
    void OnButtonSendCmd( wxCommandEvent& p_event );
    void OnButtonClearCmd( wxCommandEvent& p_event );

    void SetLuaContext( DrawSpace::LuaContext* p_luacontext );

    void Print( const dsstring& p_text );

};


#endif
