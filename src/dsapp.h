/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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
/* -*-LIC_END-*- */

#pragma once

#include "ds_types.h"
#include "jsonparser.h"
#include "renderer.h"

namespace DrawSpace
{
class App
{
protected:

    class Config : public DrawSpace::JSONParser
    {
    protected:
        //bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

    public:
        long        m_width;
        long        m_height;
        bool        m_fullscreen;
        dsstring    m_renderplugin;

        Config( long p_width, long p_height, bool p_fullscreen, bool p_multithread );

        void ParseFromFile( const dsstring& p_filepath );
    };


    static App*                             m_base_instance;
    dsstring                                m_cmdline;

    ///////////////////Parametrage Fenetre//////////////////

    HINSTANCE                               m_hinstance;
    HWND                                    m_hwnd;
    long                                    m_w_width;
    long                                    m_w_height;
    bool                                    m_w_fullscreen;
    long                                    m_w_style;
    dsstring                                m_w_title;
    Config                                  m_config;
    dsstring                                m_renderplugin;

    Interface::Renderer::Characteristics    m_renderer_characteristics;

    /////////////////Misc stuff////////////////////////////

    bool                                    m_app_ready;
    bool                                    m_keypress;
    bool                                    m_keypulse;
    long                                    m_keycode;
    bool                                    m_gl_ready;


    ///////////////////Entree souris ///////////////////////

    bool                                    m_mousemoving;
    bool                                    m_mousemovingstart;
    bool                                    m_mousewheel;
    long                                    m_mousemoving_pos;
    long                                    m_mousemoving_curr_x;
    long                                    m_mousemoving_curr_y;
    long                                    m_mousemoving_last_x;
    long                                    m_mousemoving_last_y;
    long                                    m_mouselclick_pos;
    long                                    m_mouserclick_pos;
    long                                    m_mouse_circularmode;
    long                                    m_mousewheel_delta;

    App( void );

    void process_input_events( void );
    

    //////OUTPUT : OVERRIDABLES//////////////////////////////////////////////////////

    virtual void OnRenderFrame( void ) { };

    virtual bool OnIdleAppInit( void ) 
    { 
        return true; 
    };

    virtual void OnAppInit( void ) { };
    virtual void OnClose( void ) { };

    virtual void OnKeyPress( long p_key ) 
    { 
    };
    virtual void OnEndKeyPress( long p_key ) { };

    virtual void OnKeyPulse( long p_key ) { };

    virtual void OnChar( long p_char, long p_scan ) { };

    virtual void OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy ) { };
    virtual void OnMouseWheel( long p_distance ) { };

    virtual void OnMouseLeftButtonDown( long p_xm, long p_ym ) { };
    virtual void OnMouseLeftButtonUp( long p_xm, long p_ym ) { };

    virtual void OnMouseRightButtonDown( long p_xm, long p_ym ) { };
    virtual void OnMouseRightButtonUp( long p_xm, long p_ym ) { };

    virtual void OnAppEvent( WPARAM p_wParam, LPARAM p_lParam ) { };


public:
    ~App( void );

    virtual bool InitApp( HINSTANCE p_hInstance );
    virtual void IdleApp( void );

    virtual bool InitRenderer( void );
    virtual void StopRenderer( void );

    virtual bool IsFullScreen( void ) { return m_w_fullscreen; };

    virtual HWND GetHwnd( void ) { return m_hwnd; };

    virtual void SetCmdLine( const std::string& p_cmdline ) { m_cmdline = p_cmdline; };

    virtual void GetRenderPluginName( dsstring& p_plugin );

    virtual void Quit( int p_code );


    //////INPUT : WINDOWS EVENT//////////////////////////////////////////////////////

    static void OSInputEvtLButtonUp( long p_pos );
    static void	OSInputEvtLButtonDown( long p_pos );
    static void	OSInputEvtRButtonUp( long p_pos );
    static void	OSInputEvtRButtonDown( long p_pos );
    static void	OSInputEvtKeyDown( long p_key );
    static void	OSInputEvtKeyUp( long p_key );
    static void OSInputEvtChar( long p_char, long p_scan );
    static void	OSInputEvtMouseMove( long p_pos, long p_button );
    static void OSInputEvtMouseWheel( long p_distance );
    static void	OSInputEvtApp( WPARAM p_wParam, LPARAM p_lParam );

};

};


