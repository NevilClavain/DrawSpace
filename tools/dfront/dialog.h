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

#ifndef _DIALOG_H_
#define _DIALOG_H_

#include <wx/wx.h>
#include <wx/notebook.h>

class RenderFrame;

class Dialog : public wxDialog
{
protected:
	DECLARE_EVENT_TABLE()

    RenderFrame* m_owner;

    wxNotebook*								m_notebook;

    wxBoxSizer*								m_topsizer;

	wxWindow*								m_scenes_tab;
    
    wxListBox*								m_scenes_listbox;
    wxButton*                               m_newscene_button;
   
	wxWindow*								m_scenegraph_tab;


public:

    Dialog( wxWindow* p_parent, const wxString& p_title, RenderFrame* p_owner );
    ~Dialog( void );

	void OnIdle( wxIdleEvent& p_event );
	void OnClose( wxCloseEvent& p_event );

};

#endif