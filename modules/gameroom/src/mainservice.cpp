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

#include "mainservice.h"
#include "component.h"
#include "luacontext.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;

_DECLARE_DS_LOGGER( logger, "gameroom_mainservice", NULL )

MainService::MainService( void ) :
m_console_active( false ),
m_console_current_line( 0 ),
m_meshe_import( NULL )
{
    m_console_texts.push_back( "Console: input ready" );
    m_console_texts.push_back( ">" );
    m_console_current_line++;
}

MainService::~MainService( void )
{
    if( m_meshe_import )
    {
        _DRAWSPACE_DELETE_( m_meshe_import );
    }
}

bool MainService::Init( void )
{
    //////////////recup params du service //////////////////

    ComponentList<DrawSpace::Logger::Configuration*> logconfs;
    m_owner->GetComponentsByType<DrawSpace::Logger::Configuration*>( logconfs );

    ComponentList<DrawSpace::Core::BaseCallback<void, bool>*> mouse_cbs;
    m_owner->GetComponentsByType<DrawSpace::Core::BaseCallback<void, bool>*>( mouse_cbs );

    ComponentList<DrawSpace::Core::BaseCallback<void, int>*> app_cbs;
    m_owner->GetComponentsByType<DrawSpace::Core::BaseCallback<void, int>*>( app_cbs );

    DrawSpace::Core::BaseCallback<void, bool>* mousecircularmode_cb;
    DrawSpace::Core::BaseCallback<void, bool>* mousevisible_cb;

    mousecircularmode_cb = mouse_cbs[0]->getPurpose();
    mousevisible_cb = mouse_cbs[1]->getPurpose();

    DrawSpace::Logger::Configuration* logconf = logconfs[0]->getPurpose();

    DrawSpace::Core::BaseCallback<void, int>* closeapp_cb;
    closeapp_cb = app_cbs[0]->getPurpose();


    m_mousecircularmode_cb = mousecircularmode_cb;
    m_closeapp_cb = closeapp_cb;

    ////////////////////////////////////////////////////////

        
    logconf->RegisterSink( &logger );
    logger.SetConfiguration( logconf );

    logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( logconf );

    m_systemsHub.SetLogConf( logconf );

    /////////////////////////////////////////////////////////////////////////////////

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_renderer->GetDescr( m_pluginDescr );

    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    m_meshe_import = _DRAWSPACE_NEW_( DrawSpace::Utils::AC3DMesheImport, DrawSpace::Utils::AC3DMesheImport );

    /////////////////////////////////////////////////////////////////////////////////

    LuaContext::GetInstance()->Startup();

    LuaContext::GetInstance()->Execute( "FALSE=0" );
    LuaContext::GetInstance()->Execute( "TRUE=1" );

    // type d'aspect
    LuaContext::GetInstance()->Execute( "BODY_ASPECT=0" );
    LuaContext::GetInstance()->Execute( "CAMERA_ASPECT=1" );
    LuaContext::GetInstance()->Execute( "PHYSICS_ASPECT=2" );
    LuaContext::GetInstance()->Execute( "RENDERING_ASPECT=3" );
    LuaContext::GetInstance()->Execute( "SERVICE_ASPECT=4" );
    LuaContext::GetInstance()->Execute( "TIME_ASPECT=5" );
    LuaContext::GetInstance()->Execute( "TRANSFORM_ASPECT=6" );

    // args loading shaders
    LuaContext::GetInstance()->Execute( "SHADER_COMPILED=1");
    LuaContext::GetInstance()->Execute( "SHADER_NOT_COMPILED=0");

    // time scale
    LuaContext::GetInstance()->Execute( "NORMAL_TIME=0");
    LuaContext::GetInstance()->Execute( "MUL2_TIME=1");
    LuaContext::GetInstance()->Execute( "MUL4_TIME=2");
    LuaContext::GetInstance()->Execute( "MUL10_TIME=3");
    LuaContext::GetInstance()->Execute( "MUL100_TIME=4");
    LuaContext::GetInstance()->Execute( "MUL500_TIME=5");
    LuaContext::GetInstance()->Execute( "SEC_1HOUR_TIME=6");
    LuaContext::GetInstance()->Execute( "SEC_1DAY_TIME=7");
    LuaContext::GetInstance()->Execute( "SEC_30DAYS_TIME=8");
    LuaContext::GetInstance()->Execute( "SEC_1YEAR_TIME=9");
    LuaContext::GetInstance()->Execute( "DIV2_TIME=10");
    LuaContext::GetInstance()->Execute( "DIV4_TIME=11");
    LuaContext::GetInstance()->Execute( "DIV10_TIME=12");
    LuaContext::GetInstance()->Execute( "FREEZE=13");



    LuaContext::GetInstance()->Execute( "g=Globals()" );
    LuaContext::GetInstance()->Execute( "renderer=Renderer()" );
    LuaContext::GetInstance()->Execute( "rg=RenderPassNodeGraph('rg')" );
    LuaContext::GetInstance()->Execute( "rg:create_root('final_pass')" );

    LuaContext::GetInstance()->Execute( "eg=EntityNodeGraph('eg')" );
    LuaContext::GetInstance()->Execute( "root_entity=Entity()" );

    LuaContext::GetInstance()->Execute( "root_entity:add_aspect(RENDERING_ASPECT)" );
    LuaContext::GetInstance()->Execute( "root_entity:add_aspect(TIME_ASPECT)" );
    LuaContext::GetInstance()->Execute( "root_entity:configure_timemanager(NORMAL_TIME)" );

    LuaContext::GetInstance()->Execute( "root_entity:connect_renderingaspect_rendergraph(rg)" );
    LuaContext::GetInstance()->Execute( "eg:set_root('root', root_entity )" );


    // creation cote lua de l'enum RenderState::Operation
    LuaContext::GetInstance()->Execute( "RENDERSTATE_OPE_NONE=0" );
    LuaContext::GetInstance()->Execute( "RENDERSTATE_OPE_SETCULLING=1" );
    LuaContext::GetInstance()->Execute( "RENDERSTATE_OPE_ENABLEZBUFFER=2" );
    LuaContext::GetInstance()->Execute( "RENDERSTATE_OPE_SETTEXTUREFILTERTYPE=3" );
    LuaContext::GetInstance()->Execute( "RENDERSTATE_OPE_SETVERTEXTEXTUREFILTERTYPE=4" );
    LuaContext::GetInstance()->Execute( "RENDERSTATE_OPE_SETFILLMODE=5" );
    LuaContext::GetInstance()->Execute( "RENDERSTATE_OPE_ALPHABLENDENABLE=6" );
    LuaContext::GetInstance()->Execute( "RENDERSTATE_OPE_ALPHABLENDOP=7" );
    LuaContext::GetInstance()->Execute( "RENDERSTATE_OPE_ALPHABLENDFUNC=8" );
    LuaContext::GetInstance()->Execute( "RENDERSTATE_OPE_ALPHABLENDDEST=9" );
    LuaContext::GetInstance()->Execute( "RENDERSTATE_OPE_ALPHABLENDSRC=10" );


    LuaContext::GetInstance()->Execute( "print_memsize=function() g:print('Total mem = '..g:totalmem()..' byte(s)') end" );



    

    /////////////////////////////////////////////////////////////////////////////////
    

    DrawSpace::EntityGraph::EntityNode& root_entity_node = m_entitygraphs["eg"]->GetEntityNode( "root" );
    DrawSpace::Core::Entity* root_entity = root_entity_node.GetEntity();

    RenderingAspect* rendering_aspect = root_entity->GetAspect<RenderingAspect>();

    rendering_aspect->AddImplementation( &m_textRender );
    
    rendering_aspect->AddComponent<std::vector<TextRenderingAspectImpl::TextDisplay>>( "console_lines" );


    create_console_quad();

    // ajout du quad console a la scene
    m_quadEntityNode = root_entity_node.AddChild( &m_quadEntity );

    m_quadRender.RegisterToRendering( m_rendergraphs["rg"]->GetRenderGraph() );
    m_rendergraphs["rg"]->GetRenderGraph().PushSignal_UpdatedRenderingQueues();

    /////////////////////////////////////////////////////////////////////////////////

    m_systemsHub.Init( &m_entitygraphs["eg"]->GetEntityGraph() );
    m_entitygraphs["eg"]->GetEntityGraph().PushSignal_RenderSceneBegin();

    //set_mouse_circular_mode( true );

    _DSDEBUG( logger, dsstring("MainService : startup...") );

    return true;
}


void MainService::Run( void )
{
    m_systemsHub.Run( &m_entitygraphs["eg"]->GetEntityGraph() );
    
    DrawSpace::EntityGraph::EntityNode& root_entity_node = m_entitygraphs["eg"]->GetEntityNode( "root" );
    DrawSpace::Core::Entity* root_entity = root_entity_node.GetEntity();
    RenderingAspect* rendering_aspect = root_entity->GetAspect<RenderingAspect>();

    rendering_aspect->GetComponent<std::vector<TextRenderingAspectImpl::TextDisplay>>( "console_lines" )->getPurpose().clear();
    if( m_console_active )
    {
        print_console_content();
    }

    /////////////////////////////////////////////////////

    execute_lua_run_cbs();
}

void MainService::Release( void )
{
    _DSDEBUG( logger, dsstring("MainService : shutdown...") );

    m_systemsHub.Release( &m_entitygraphs["eg"]->GetEntityGraph() );

    LuaContext::GetInstance()->Shutdown();
}

void MainService::print_console_content( void )
{
    DrawSpace::EntityGraph::EntityNode& root_entity_node = m_entitygraphs["eg"]->GetEntityNode( "root" );
    DrawSpace::Core::Entity* root_entity = root_entity_node.GetEntity();
    RenderingAspect* rendering_aspect = root_entity->GetAspect<RenderingAspect>();

    if( m_console_texts.size() <= m_console_max_lines_display )
    {
        for( size_t i = 0; i < m_console_texts.size(); i++ )
        {
            TextRenderingAspectImpl::TextDisplay myline( 15, m_console_y_pos + ( i * 15 ), 10, 70, 10, m_console_texts[i] );
            rendering_aspect->GetComponent<std::vector<TextRenderingAspectImpl::TextDisplay>>( "console_lines" )->getPurpose().push_back( myline );
        }
    }
    else
    {
        for( size_t i = 0; i < m_console_max_lines_display; i++ )
        {
            TextRenderingAspectImpl::TextDisplay myline( 15, m_console_y_pos + ( i * 15 ), 10, 70, 10, m_console_texts[m_console_texts.size() - m_console_max_lines_display + i] );

            rendering_aspect->GetComponent<std::vector<TextRenderingAspectImpl::TextDisplay>>( "console_lines" )->getPurpose().push_back( myline );
        }
    }
    
}

void MainService::OnKeyPress( long p_key )
{
    if( m_console_active )
    {
        return;
    }
}

void MainService::OnEndKeyPress( long p_key )
{
    if( m_console_active )
    {
        return;
    }
}

void MainService::OnKeyPulse( long p_key )
{
}

void MainService::OnChar( long p_char, long p_scan )
{
    if( 178 == p_char )
    {
        if( m_console_active )
        {
            m_console_active = false;       
        }
        else
        {
            m_console_active = true;            
        }

        RenderingAspect* rendering_aspect = m_quadEntity.GetAspect<RenderingAspect>();
        RenderingNode* quad_pass = rendering_aspect->GetComponent<QuadRenderingAspectImpl::PassSlot>( "finalpass_slot" )->getPurpose().GetRenderingNode();
        quad_pass->SetDrawingState( m_console_active );
     
        return;
    }

    if( m_console_active )
    {
        if( 8 == p_char )
        {        
            if( m_console_texts[m_console_current_line].size() > 1 )
            {
                char comment[4096];

                strcpy( comment, m_console_texts[m_console_current_line].c_str() );
                comment[m_console_texts[m_console_current_line].size()-1] = 0;

                m_console_texts[m_console_current_line] = comment;
            }        
        }
        else if( 13 == p_char )
        {
            char comment[4096];

            strcpy( comment, m_console_texts[m_console_current_line].c_str() );
            char* cmd = comment;
            cmd++; // sauter le '>' 
            if( strcmp( cmd, "" ) )
            {
                process_console_command( cmd );
            }
            m_console_texts.push_back( ">" );
            m_console_current_line++;    
        }
        else
        {
            m_console_texts[m_console_current_line] += p_char;
        }
    }
}

void MainService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
}

void MainService::OnMouseWheel( long p_delta )
{
}

void MainService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
}

void MainService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
}

void MainService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
}

void MainService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
}

void MainService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}

void MainService::process_console_command( const dsstring& p_cmd )
{
    if( false == LuaContext::GetInstance()->Execute( p_cmd ) )
    {
        dsstring lua_err = LuaContext::GetInstance()->GetLastError();
        print_console_line( lua_err );
    }
}

void MainService::print_console_line( const dsstring& p_text )
{
    m_console_texts.push_back( p_text );
    m_console_current_line++;
}


void MainService::set_mouse_circular_mode( bool p_state )
{
    if( m_mousecircularmode_cb )
    {
        (*m_mousecircularmode_cb)( p_state );        
    }
}

void MainService::create_console_quad( void )
{
    RenderingAspect* rendering_aspect = m_quadEntity.AddAspect<RenderingAspect>();
    rendering_aspect->AddImplementation( &m_quadRender );

    rendering_aspect->AddComponent<QuadRenderingAspectImpl::PassSlot>( "finalpass_slot", "final_pass" );
    RenderingNode* quad_pass = rendering_aspect->GetComponent<QuadRenderingAspectImpl::PassSlot>( "finalpass_slot" )->getPurpose().GetRenderingNode();
    quad_pass->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    quad_pass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "color.vso", true ) ) );
    quad_pass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "color.pso", true ) ) );

    quad_pass->GetFx()->GetShader( 0 )->LoadFromFile();
    quad_pass->GetFx()->GetShader( 1 )->LoadFromFile();

    quad_pass->AddShaderParameter( 1, "color", 0 );
    quad_pass->SetShaderRealVector( "color", Vector( 0.5, 0.5, 0.5, 0.85 ) );

    RenderStatesSet quadpass_rss;
    quadpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    
    quadpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    quadpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    quadpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    quadpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha" ) );
    quadpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );


    quadpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    quadpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    quad_pass->GetFx()->SetRenderStates( quadpass_rss );

    quad_pass->SetOrderNumber( 11000 );

    TransformAspect* transform_aspect = m_quadEntity.AddAspect<TransformAspect>();

    transform_aspect->SetImplementation( &m_quadTransformer );

    
    transform_aspect->AddComponent<Matrix>( "quad_scaling" );
    transform_aspect->GetComponent<Matrix>( "quad_scaling" )->getPurpose().Scale( 2.0, 2.0, 1.0 );
    

    transform_aspect->AddComponent<Matrix>( "quad_pos" );
    transform_aspect->GetComponent<Matrix>( "quad_pos" )->getPurpose().Translation( 0.0, 0.0, -1.0 );

    quad_pass->SetDrawingState( m_console_active );
}


void MainService::execute_lua_run_cbs( void )
{
    //for( size_t i = 0; i < m_run_lua_callbacks.size(); i++ )
    for( auto it = m_run_lua_callbacks.begin(); it != m_run_lua_callbacks.end(); ++it )
    {
        //LuaContext::GetInstance()->CallLuaAppRunFunc( m_run_lua_callbacks[i] );
        LuaContext::GetInstance()->CallLuaAppRunFunc( it->second );
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainService::RegisterRenderGraph( const std::string& p_id, LuaClass_RenderPassNodeGraph* p_rg )
{
    m_rendergraphs[p_id] = p_rg;
}

void MainService::UnregisterRenderGraph( const std::string& p_id )
{
    if( m_rendergraphs.count( p_id ) )
    {
        m_rendergraphs.erase( p_id );
    }
}

void MainService::RegisterEntityGraph( const std::string& p_id, LuaClass_EntityNodeGraph* p_eg )
{
    m_entitygraphs[p_id] = p_eg;
}

void MainService::UnregisterEntityGraph( const std::string& p_id )
{
    if( m_entitygraphs.count( p_id ) )
    {
        m_entitygraphs.erase( p_id );
    }
}

void MainService::RegisterRunCallback( const dsstring& p_id, int p_regindex )
{
    m_run_lua_callbacks[p_id] = p_regindex;
}

int MainService::UnregisterRunCallback( const dsstring& p_id )
{
    int index = -1;
    if( m_run_lua_callbacks.count( p_id ) )
    {
        index = m_run_lua_callbacks[p_id];
        m_run_lua_callbacks.erase( p_id );       
    }
    return index;
}

void MainService::RequestClose( void )
{
    (*m_closeapp_cb)( 0 );
}

void MainService::RequestClearConsole( void )
{
    m_console_texts.clear();
    m_console_texts.push_back( "Command input ready" );
    m_console_current_line = 0;
}

void MainService::RequestConsolePrint( const dsstring& p_msg )
{
    print_console_line( p_msg );
}


void MainService::RequestLuaFileExec( const dsstring& p_path )
{
    if( false == LuaContext::GetInstance()->ExecuteFromFile( p_path ) )
    {
        dsstring lua_err = LuaContext::GetInstance()->GetLastError();
        print_console_line( lua_err );
    }
}

void MainService::RequestMemAllocDump( void )
{
    MemAlloc::GetInstance()->DumpContent();
}