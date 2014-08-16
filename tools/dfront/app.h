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

#ifndef _APP_H_
#define _APP_H_

#include <wx/wx.h>

#include "renderframe.h"
#include "drawspace.h"


class DFrontApp : public wxApp
{
protected:

    class Config : public DrawSpace::Utils::Parser
    {
    protected:
        bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

    public:
        long        m_width;
        long        m_height;
        bool        m_fullscreen;
        dsstring    m_renderplugin;

        Config( long p_width, long p_height, bool p_fullscreen );
    };

    DrawSpace::LuaContext               m_luacontext;

    RenderFrame*                        m_frame;

    ////////////////////////////////////////////////

    HWND                                m_hwnd;


    long                                m_w_width;
    long                                m_w_height;
    bool                                m_w_fullscreen;
    dsstring                            m_renderplugin;
    Config                              m_config;
    dsstring                            m_w_title;

    ////////////////////////////////////////////////

    bool                                m_app_ready;

    bool                                init_renderer( void );
    bool                                load_renderer_plugin( const dsstring& p_file );

public:
	DFrontApp( void );
	virtual ~DFrontApp( void );

	virtual bool OnInit( void );
    virtual int OnExit( void );

    //virtual int FilterEvent( wxEvent& p_event );
};

DECLARE_APP( DFrontApp )




#endif