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

#ifndef _APP_H_
#define _APP_H_

#include <wx/wx.h>
#include <wx/cmdline.h>
#include "drawspace.h"
#include "BasicSceneMainFrame.h"
#include "scripting.h"

static const wxCmdLineEntryDesc cmdLineDesc[] =
{
    { wxCMD_LINE_OPTION, "r", "resource_file", "resources description file", wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_OPTION, "s", "script_file", "scripting file", wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_NONE }
};

class DFrontApp : public wxApp
{
protected:



    //RenderFrame*                        m_frame;

    BasicSceneMainFrame*                m_mainframe;

    ////////////////////////////////////////////////

    HWND                                m_hwnd;


    long                                m_w_width;
    long                                m_w_height;
    bool                                m_w_fullscreen;
    dsstring                            m_renderplugin;

    dsstring                            m_w_title;

    ////////////////////////////////////////////////

    bool                                m_app_ready;


    



    dsstring                            m_resource_filepath;

    dsstring                            m_script_filepath;
 

    bool                                init_renderer( void );
    bool                                load_renderer_plugin( const dsstring& p_file );
    bool                                load_scripting_plugin( const dsstring& p_file );

public:
	DFrontApp( void );
	virtual ~DFrontApp( void );

	virtual bool OnInit( void );
    virtual int OnExit( void );

    virtual void OnInitCmdLine( wxCmdLineParser& p_parser );
    virtual bool OnCmdLineParsed( wxCmdLineParser& p_parser );

    virtual bool OnExceptionInMainLoop( void );
};

DECLARE_APP( DFrontApp )




#endif
