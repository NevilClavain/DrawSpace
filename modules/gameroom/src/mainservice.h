/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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


#ifndef _MAINSERVICE_H_
#define _MAINSERVICE_H_

#include "drawspace.h"
#include "serviceaspectimpl.h"
#include "crtp_singleton.h"

#include "luaclass_renderpassnodegraph.h"
#include "luaclass_entitynodegraph.h"
#include "luaclass_entity.h"

class MainService : public DrawSpace::Interface::AspectImplementations::ServiceAspectImpl, public BaseSingleton<MainService>
{
protected:

    using GUIWidgetPushButtonClickedCallback = DrawSpace::Core::CallBack2<MainService, void, const dsstring&, const dsstring&>;

    GUIWidgetPushButtonClickedCallback                                              m_guiwidgetpushbuttonclicked_cb;

    DrawSpace::Core::BaseCallback<void, bool>*                                      m_mousevisible_cb;

    static const int                                                                m_console_max_lines_display = 22; //31;
    int                                                                             m_console_y_pos;// = 500;//50;

    bool                                                                            m_request_lua_reset;

    DrawSpace::Interface::Renderer*                                                 m_renderer;
    dsstring                                                                        m_pluginDescr;

    DrawSpace::Utils::AC3DMesheImport                                               m_meshe_import;

    std::unordered_map<dsstring, LuaClass_RenderPassNodeGraph*>                     m_rendergraphs; // table des rendergraph
    std::unordered_map<dsstring, LuaClass_EntityNodeGraph*>                         m_entitygraphs;


    
    DrawSpace::Core::Entity                                                         m_quadEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_quadEntityNode;

    DrawSpace::Systems::Hub                                                         m_systemsHub;
    DrawSpace::AspectImplementations::TextRenderingAspectImpl                       m_textRender;
    DrawSpace::AspectImplementations::QuadRenderingAspectImpl                       m_quadRender;

    DrawSpace::AspectImplementations::RawTransformAspectImpl                        m_quadTransformer;

    DrawSpace::Core::BaseCallback<void, bool>*                                      m_mousecircularmode_cb;
    DrawSpace::Core::BaseCallback<void, int>*                                       m_closeapp_cb;
    
    std::map<dsstring, int>                                                         m_run_lua_callbacks;
    std::map<dsstring, int>                                                         m_keypress_lua_callbacks;
    std::map<dsstring, int>                                                         m_endkeypress_lua_callbacks;
    std::map<dsstring, int>                                                         m_onchar_lua_callbacks;
    std::map<dsstring, int>                                                         m_mousemove_lua_callbacks;
    std::map<dsstring, int>                                                         m_mouseleftbuttondown_lua_callbacks;
    std::map<dsstring, int>                                                         m_mouseleftbuttonup_lua_callbacks;
    std::map<dsstring, int>                                                         m_mouserightbuttondown_lua_callbacks;
    std::map<dsstring, int>                                                         m_mouserightbuttonup_lua_callbacks;
    std::map<dsstring, int>                                                         m_guipushbuttonclicked_lua_callbacks;



    //////////////gestion de la console///////////////////
    bool                                                                            m_console_ready;
    bool                                                                            m_console_active;
    std::vector<dsstring>                                                           m_console_texts;
    int                                                                             m_console_current_line;

    bool                                                                            m_console_newline;

    bool                                                                            m_print_from_command;


    void on_guipushbutton_clicked( const dsstring& p_layout, const dsstring& p_widget_id );

    void set_mouse_circular_mode( bool p_state );

    void create_skybox( void );
    void create_ground( void );
    void create_camera( void );
    void create_console_quad( void );

    void process_console_command( const dsstring& p_cmd );
    void print_console_line( const dsstring& p_text );
    void print_console_content( void );


    void execute_lua_run_cbs( void );

    void buil_lua_prerequisites( void );

public:

    MainService( void );
    ~MainService( void );

    bool Init( void );
    void Run( void );
    void Release( void );

    void OnKeyPress( long p_key );
    void OnEndKeyPress( long p_key );
    void OnKeyPulse( long p_key );
    void OnChar( long p_char, long p_scan );
    void OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy );
    void OnMouseWheel( long p_delta );
    void OnMouseLeftButtonDown( long p_xm, long p_ym );
    void OnMouseLeftButtonUp( long p_xm, long p_ym );
    void OnMouseRightButtonDown( long p_xm, long p_ym );
    void OnMouseRightButtonUp( long p_xm, long p_ym );
    void OnAppEvent( WPARAM p_wParam, LPARAM p_lParam );

    //////////////////////////////////////////////////////////////////////////

    void RegisterRenderGraph( const std::string& p_id, LuaClass_RenderPassNodeGraph* p_rg );
    void UnregisterRenderGraph( const std::string& p_id );
    void RegisterEntityGraph( const std::string& p_id, LuaClass_EntityNodeGraph* p_eg );
    void UnregisterEntityGraph( const std::string& p_id );

    void RegisterRunCallback( const dsstring& p_id, int p_regindex );
    int UnregisterRunCallback( const dsstring& p_id );

    void RegisterKeyPressCallback( const dsstring& p_id, int p_regindex );
    int UnregisterKeyPressCallback( const dsstring& p_id );

    void RegisterEndKeyPressCallback( const dsstring& p_id, int p_regindex );
    int UnregisterEndKeyPressCallback( const dsstring& p_id );

    void RegisterOnCharCallback( const dsstring& p_id, int p_regindex );
    int UnregisterOnCharCallback( const dsstring& p_id );

    void RegisterMouseMoveCallback( const dsstring& p_id, int p_regindex );
    int UnregisterMouseMoveCallback( const dsstring& p_id );

    void RegisterMouseLeftButtonDownCallback( const dsstring& p_id, int p_regindex );
    int UnregisterMouseLeftButtonDownCallback( const dsstring& p_id );

    void RegisterMouseLeftButtonUpCallback( const dsstring& p_id, int p_regindex );
    int UnregisterMouseLeftButtonUpCallback( const dsstring& p_id );

    void RegisterMouseRightButtonDownCallback( const dsstring& p_id, int p_regindex );
    int UnregisterMouseRightButtonDownCallback( const dsstring& p_id );

    void RegisterMouseRightButtonUpCallback( const dsstring& p_id, int p_regindex );
    int UnregisterMouseRightButtonUpCallback( const dsstring& p_id );

    void RegisterGuiPushButtonClickedCallback( const dsstring& p_id, int p_regindex );
    int UnregisterGuiPushButtonClickedCallback( const dsstring& p_id );

    DrawSpace::Interface::MesheImport* GetMesheImport( void );

    void RequestClose( void );
    void RequestClearConsole( void );
    void RequestConsolePrint( const dsstring& p_msg );
    int RequestLuaFileExec( const dsstring& p_path, dsstring& p_err );

    void RequestMemAllocDump( void );

    void RequestGuiDisplay( bool p_display );

    void RequestLuaStackReset();

    void RequestMouseCursorDisplayState( bool p_display );
    void RequestMouseCursorCircularMode( bool p_state );


    void RequestSignalRenderSceneBegin( const dsstring& p_entitygraph_id );
    void RequestSignalRenderSceneEnd( const dsstring& p_entitygraph_id );

    

    friend class BaseSingleton<MainService>;
};

#endif
