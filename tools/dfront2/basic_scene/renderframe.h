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
#include <DrawSpace.h>

#include "BasicScenePanel.h"

class RenderFrame : public wxFrame
{
public:
    static wxString                     m_caption;
    static wxSize                       m_size;

protected:
	DECLARE_EVENT_TABLE()

    static RenderFrame*                 m_instance;
    bool                                m_gl_ready;
    DrawSpace::Interface::MesheImport*  m_meshe_import;

    BasicScenePanel*                    m_panel;
    
    void                                on_render_frame( void );
    
    RenderFrame( void );

public:
	virtual ~RenderFrame( void );

    static RenderFrame* GetInstance( void )
    {
        if( !m_instance )
        {
            m_instance = new RenderFrame(); 
        }
        return m_instance;
    }

    void SetGlReady( bool p_ready );
	void OnIdle( wxIdleEvent& p_event );
	void OnClose( wxCloseEvent& p_event );
    void SetMesheImport( DrawSpace::Interface::MesheImport* p_import );

};


#endif