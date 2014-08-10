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

#ifndef _FRAME_H_
#define _FRAME_H_

#include <wx/wx.h>
#include <vector>

#include "dialog.h"
#include "console.h"

#include "scene.h"

class RenderFrame : public wxFrame
{
protected:
	DECLARE_EVENT_TABLE()

    Dialog*                             m_dialog;
    ConsoleDialog*                      m_console_dialog;
    bool                                m_gl_ready;

    std::vector<IScene*>                m_scenes;

    void                                on_render_frame( void );

public:

	RenderFrame( const wxString& p_caption, const wxSize& p_size );
	virtual ~RenderFrame( void );

    void SetGlReady( bool p_ready );
	void OnIdle( wxIdleEvent& p_event );
	void OnClose( wxCloseEvent& p_event );
};


#endif