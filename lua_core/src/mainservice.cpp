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

#include "mainservice.h"

#include "module_root.h"
#include "component.h"
#include "luacontext.h"
#include "serviceaspect.h"
#include "transformaspect.h"
#include "logconf.h"
#include "callback.h"
#include "matrix.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Systems;

static DrawSpace::Logger::Sink logger("gameroom_mainservice");

#ifdef _DEBUG
static const dsstring console_welcome = "Console: input ready (debug build)";
#else
static const dsstring console_welcome = "Console: input ready (release build)";
#endif

extern DrawSpace::Logger::Sink aspect_logger;
extern DrawSpace::Logger::Sink rs_logger;       //resource system logger
extern DrawSpace::Logger::Sink rd_logger;       //renderingqueue system logger
extern DrawSpace::Logger::Sink bmt_logger;

extern DrawSpace::Logger::Sink runner_logger;

MainService::MainService( void ) :
m_console_ready( false ),
m_console_active( false ),
m_console_current_line( 0 ),
m_request_lua_reset( false ),
m_guiwidgetpushbuttonclicked_cb( this, &MainService::on_guipushbutton_clicked ),
m_animation_events_cb( this, &MainService::on_animation_event ),
m_resource_events_cb( this, &MainService::on_resource_event )
{
    m_console_texts.push_back( console_welcome );
    m_console_texts.push_back( ">" );
    m_console_current_line++;
	m_caret_pos_from_end = 0;
	m_recall_command_index = 0;
}

MainService::~MainService( void )
{
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

    const auto logconf{ logconfs[0]->getPurpose() };

    DrawSpace::Core::BaseCallback<void, int>* closeapp_cb;
    closeapp_cb = app_cbs[0]->getPurpose();

    m_mousevisible_cb = mousevisible_cb;
    m_mousecircularmode_cb = mousecircularmode_cb;
    m_closeapp_cb = closeapp_cb;

    ////////////////////////////////////////////////////////
        
    logconf->registerSink( &logger );
    logger.SetConfiguration( logconf );

    logconf->registerSink(&aspect_logger);
    aspect_logger.SetConfiguration(logconf);

    logconf->registerSink(&rs_logger);
    rs_logger.SetConfiguration(logconf);

    logconf->registerSink(&bmt_logger);
    bmt_logger.SetConfiguration(logconf);


    logconf->registerSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( logconf );

    logconf->registerSink(&rd_logger);
    rd_logger.SetConfiguration(logconf);

    logconf->registerSink(&runner_logger);
    runner_logger.SetConfiguration(logconf);

    m_systemsHub.SetLogConf( logconf );

    m_logconf = logconf;

    /////////////////////////////////////////////////////////////////////////////////

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_renderer->GetDescr( m_pluginDescr );

    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    m_renderer->GUI_RegisterPushButtonEventClickedHandler( &m_guiwidgetpushbuttonclicked_cb );



    DrawSpace::Interface::Renderer::Characteristics characteristics;
    m_renderer->GetRenderCharacteristics( characteristics );


    m_console_y_pos = characteristics.height_resol * 0.5;
    m_console_y_pos += 50;

    ////////////////////////////////////////////////////////

	create_console_quad(&m_systemsHub.GetSystem<DrawSpace::Systems::ResourcesSystem>("ResourcesSystem"));

    /////////////////////////////////////////////////////////////////////////////////

    LuaContext::getInstance()->Startup();
    buil_lua_prerequisites();
   
    /////////////////////////////////////////////////////////////////////////////////

    auto& animationsystem{ m_systemsHub.GetSystem<DrawSpace::Systems::AnimationsSystem>("AnimationsSystem") };
	animationsystem.RegisterAnimationEvtHandler(&m_animation_events_cb);

    auto& resourcesystem{ m_systemsHub.GetSystem<DrawSpace::Systems::ResourcesSystem>("ResourcesSystem") };
    resourcesystem.RegisterEventHandler(&m_resource_events_cb);

    m_systemsHub.StartupRunner();

    _DSDEBUG( logger, dsstring("MainService : startup...") );

    return true;
}


void MainService::Run( void )
{
    m_systemsHub.Run( &m_entitygraphs["eg"]->GetEntityGraph() );
    
    DrawSpace::EntityGraph::EntityNode& root_entity_node = m_entitygraphs["eg"]->GetEntityNode( "root" );
    DrawSpace::Core::Entity* root_entity = root_entity_node.GetEntity();
    RenderingAspect* rendering_aspect = root_entity->GetAspect<RenderingAspect>();

    rendering_aspect->GetComponent<std::vector<StringRenderingAspectImpl::TextDisplay>>( "console_lines" )->getPurpose().clear();
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
    m_systemsHub.ShutdownRunner();
    LuaContext::getInstance()->Shutdown();
    m_systemsHub.ReleaseAssets();
}

void MainService::print_console_content( void )
{
	build_console_caret_line();
    DrawSpace::EntityGraph::EntityNode& root_entity_node = m_entitygraphs["eg"]->GetEntityNode( "root" );
    DrawSpace::Core::Entity* root_entity = root_entity_node.GetEntity();
    RenderingAspect* rendering_aspect = root_entity->GetAspect<RenderingAspect>();

    if( m_console_texts.size() <= m_console_max_lines_display )
    {
        for( size_t i = 0; i < m_console_texts.size(); i++ )
        {
            StringRenderingAspectImpl::TextDisplay myline( 15, m_console_y_pos + ( i * 15 ), 170, 170, 170, m_console_texts[i] );
            rendering_aspect->GetComponent<std::vector<StringRenderingAspectImpl::TextDisplay>>( "console_lines" )->getPurpose().push_back( myline );
        }
		// display caret over last line
		StringRenderingAspectImpl::TextDisplay caret_line_display(15, m_console_y_pos + ((m_console_texts.size() - 1) * 15) + 2, 170, 170, 170, m_console_caret_line);
		rendering_aspect->GetComponent<std::vector<StringRenderingAspectImpl::TextDisplay>>("console_lines")->getPurpose().push_back(caret_line_display);

    }
    else
    {
        for( size_t i = 0; i < m_console_max_lines_display; i++ )
        {
            StringRenderingAspectImpl::TextDisplay myline( 15, m_console_y_pos + ( i * 15 ), 170, 170, 170, m_console_texts[m_console_texts.size() - m_console_max_lines_display + i] );
            rendering_aspect->GetComponent<std::vector<StringRenderingAspectImpl::TextDisplay>>( "console_lines" )->getPurpose().push_back( myline );
        }
		// display caret over last line
		StringRenderingAspectImpl::TextDisplay caret_line_display(15, m_console_y_pos + ((m_console_max_lines_display - 1) * 15) + 2, 170, 170, 170, m_console_caret_line);
		rendering_aspect->GetComponent<std::vector<StringRenderingAspectImpl::TextDisplay>>("console_lines")->getPurpose().push_back(caret_line_display);
    }    
}

void MainService::build_console_caret_line(void)
{
	dsstring padding;
	dsstring current_line = m_console_texts[m_console_current_line];
	
	for (size_t i = 0; i < current_line.size() - m_caret_pos_from_end; i++)
	{
		padding = padding + " ";
	}
	padding = padding + "_";
	m_console_caret_line = padding;
}

void MainService::OnKeyPress( long p_key )
{
    if( m_console_active )
    {
    }
	else
	{
		for (auto it = m_keypress_lua_callbacks.begin(); it != m_keypress_lua_callbacks.end(); ++it)
		{
			LuaContext::getInstance()->CallLuaFunc(it->second, p_key);
		}
	}
}

void MainService::OnEndKeyPress( long p_key )
{
    if( m_console_active )
    {
		if (VK_LEFT == p_key)
		{
			dsstring current_line = m_console_texts[m_console_current_line];
			if (m_caret_pos_from_end < current_line.size() - 1) // - 1 to ignore '>' first char
			{
				m_caret_pos_from_end++;
			}
		}
		else if (VK_RIGHT == p_key)
		{		
			if (0 < m_caret_pos_from_end)
			{
				m_caret_pos_from_end--;
			}
		}
		else if (VK_HOME == p_key)
		{
			dsstring current_line = m_console_texts[m_console_current_line];
			m_caret_pos_from_end = current_line.size() - 1; // - 1 to ignore '>' first char
		}
		else if (VK_END == p_key)
		{
			m_caret_pos_from_end = 0;
		}
		else if (VK_DELETE == p_key)
		{
			size_t pos_target = m_console_texts[m_console_current_line].size() - m_caret_pos_from_end;
			m_console_texts[m_console_current_line].erase(pos_target, 1);

			if (m_caret_pos_from_end > 0)
			{
				m_caret_pos_from_end--;
			}
		}
		else if (VK_UP == p_key)
		{			
			m_caret_pos_from_end = 0;

			if (m_recall_command_index < m_commands_mem.size())
			{				
				dsstring command = m_commands_mem[m_commands_mem.size() - m_recall_command_index - 1];
				m_console_texts[m_console_current_line] = dsstring(">") + command;
				m_recall_command_index++;
			}
		}
		else if (VK_DOWN == p_key)
		{
			m_caret_pos_from_end = 0;

			if (m_recall_command_index > 0)
			{
				m_recall_command_index--;
				dsstring command = m_commands_mem[m_commands_mem.size() - m_recall_command_index - 1];
				m_console_texts[m_console_current_line] = dsstring(">") + command;
			}
		}
	}
	else
	{
		for (auto it = m_endkeypress_lua_callbacks.begin(); it != m_endkeypress_lua_callbacks.end(); ++it)
		{
			LuaContext::getInstance()->CallLuaFunc(it->second, p_key);
		}
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
        rendering_aspect->GetComponent<bool>( "draw" )->getPurpose() = m_console_active;

        return;
    }

    if( m_console_active )
    {
		if (8 == p_char)
		{
			if(m_caret_pos_from_end < m_console_texts[m_console_current_line].size() - 1) // -1 for the '>'
			{ 
				size_t pos_target = m_console_texts[m_console_current_line].size() - m_caret_pos_from_end;
				m_console_texts[m_console_current_line].erase(pos_target - 1, 1);
			}
		}
		else if (13 == p_char)
		{
			char comment[4096];

			strcpy(comment, m_console_texts[m_console_current_line].c_str());
			char* cmd = comment;
			cmd++; // sauter le '>' 
			if (strcmp(cmd, ""))
			{
				m_print_from_command = true;
				process_console_command(cmd);
				m_commands_mem.push_back(cmd);
				m_caret_pos_from_end = 0;
				m_console_newline = true;
			}

			if (m_console_newline)
			{
				m_console_texts.push_back(">");
				m_console_current_line++;

				m_console_newline = false;
			}

			m_print_from_command = false;			
		}
		else
		{
			size_t pos_target = m_console_texts[m_console_current_line].size() - m_caret_pos_from_end;
			m_console_texts[m_console_current_line].insert(pos_target, 1, (char)p_char);
		}
    }
    else
    {
        for( auto it = m_onchar_lua_callbacks.begin(); it != m_onchar_lua_callbacks.end(); ++it )
        {
            LuaContext::getInstance()->CallLuaFunc( it->second, p_char, p_scan );
        }       
    }
}

void MainService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    if( m_console_active )
    {
        return;
    }
    for( auto it = m_mousemove_lua_callbacks.begin(); it != m_mousemove_lua_callbacks.end(); ++it )
    {
        LuaContext::getInstance()->CallLuaFunc( it->second, p_xm, p_ym, p_dx, p_dy );
    }
}

void MainService::OnMouseWheel( long p_delta )
{
}

void MainService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    if( m_console_active )
    {
        return;
    }
    for( auto it = m_mouseleftbuttondown_lua_callbacks.begin(); it != m_mouseleftbuttondown_lua_callbacks.end(); ++it )
    {
        LuaContext::getInstance()->CallLuaFunc( it->second, p_xm, p_ym );
    }
}

void MainService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    if( m_console_active )
    {
        return;
    }
    for( auto it = m_mouseleftbuttonup_lua_callbacks.begin(); it != m_mouseleftbuttonup_lua_callbacks.end(); ++it )
    {
        LuaContext::getInstance()->CallLuaFunc( it->second, p_xm, p_ym );
    }
}

void MainService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    if( m_console_active )
    {
        return;
    }
    for( auto it = m_mouserightbuttondown_lua_callbacks.begin(); it != m_mouserightbuttondown_lua_callbacks.end(); ++it )
    {
        LuaContext::getInstance()->CallLuaFunc( it->second, p_xm, p_ym );
    }
}

void MainService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    if( m_console_active )
    {
        return;
    }
    for( auto it = m_mouserightbuttonup_lua_callbacks.begin(); it != m_mouserightbuttonup_lua_callbacks.end(); ++it )
    {
        LuaContext::getInstance()->CallLuaFunc( it->second, p_xm, p_ym );
    }
}

void MainService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}

void MainService::on_guipushbutton_clicked( const dsstring& p_layout, const dsstring& p_widget_id )
{
    for( auto it = m_guipushbuttonclicked_lua_callbacks.begin(); it != m_guipushbuttonclicked_lua_callbacks.end(); ++it )
    {
        LuaContext::getInstance()->CallLuaFunc( it->second, p_layout, p_widget_id );
    }
}

void MainService::on_animation_event(const dsstring& p_event_id, AnimationsSystem::AnimationEvent p_event, const dsstring& p_animation_name)
{
	for (auto it = m_animationevent_lua_callbacks.begin(); it != m_animationevent_lua_callbacks.end(); ++it)
	{
		LuaContext::getInstance()->CallLuaFunc(it->second, p_event_id, p_event, p_animation_name);
	}
}

void MainService::on_resource_event(DrawSpace::Systems::ResourcesSystem::ResourceEvent p_event, const dsstring& p_resource, const dsstring& p_context)
{
    for (auto it = m_resourceevent_lua_callbacks.begin(); it != m_resourceevent_lua_callbacks.end(); ++it)
    {
        LuaContext::getInstance()->CallLuaFunc(it->second, static_cast<int>(p_event), p_resource, p_context);
    }
}

void MainService::process_console_command( const dsstring& p_cmd )
{
    if( false == LuaContext::getInstance()->Execute( p_cmd ) )
    {
        dsstring lua_err = LuaContext::getInstance()->GetLastError();

		// cleanup error message : remove CR chars
		lua_err.erase(std::remove(lua_err.begin(), lua_err.end(), 0x0d), lua_err.end());
		lua_err.erase(std::remove(lua_err.begin(), lua_err.end(), 0x0a), lua_err.end());

		print_console_line(lua_err);
    }
    else
    {
        if( m_request_lua_reset )
        {
            LuaContext::getInstance()->Execute( "root_entity:release_timemanager()" );

            DrawSpace::EntityGraph::EntityNode& root_entity_node = m_entitygraphs["eg"]->GetEntityNode( "root" );
            DrawSpace::Core::Entity* root_entity = root_entity_node.GetEntity();
            RenderingAspect* rendering_aspect = root_entity->GetAspect<RenderingAspect>();
            rendering_aspect->RemoveComponent<std::vector<StringRenderingAspectImpl::TextDisplay>>( "console_lines" );

            LuaContext::getInstance()->Shutdown();
            LuaContext::getInstance()->Startup();
            buil_lua_prerequisites();
            m_request_lua_reset = false;
        }
    }
}

void MainService::print_console_line( const dsstring& p_text )
{
    if( !m_console_ready )
    {
        return;
    }

    m_console_texts.push_back( p_text );
    m_console_current_line++;

    if( m_print_from_command )
    {
        // origine de la chaine d'appels : execution d'une commande lua depuis la console
        m_console_newline = true;
    }
    else
    {
        // on passe ici par exemple, typiquement depuis un g:print() appele dans une callback gui d'un script lua

        m_console_texts.push_back( ">" );
        m_console_current_line++;
    }

	m_caret_pos_from_end = 0;
}


void MainService::set_mouse_circular_mode( bool p_state )
{
    if( m_mousecircularmode_cb )
    {
        (*m_mousecircularmode_cb)( p_state );        
    }
}

void MainService::create_console_quad(DrawSpace::Systems::ResourcesSystem* p_res_system)
{
    static const dsstring console_shader_ps("color_ps.hlsl");
    static const dsstring console_shader_vs("color_vs.hlsl");

    // parametrage localisation des shaders pour pouvoir creer la console
    Shader::EnableShadersDescrInFinalPath( true );
    Shader::SetRootPath( "console_data/shaders_bank" ); // temporaire, a supprimer quand tout lua stack utilisera le resource system

    DrawSpace::Systems::ResourcesSystem::SetShadersRootPath("console_data/shaders_bank");
    DrawSpace::Systems::ResourcesSystem::EnableShadersDescrInFinalPath(TRUE);

    RenderingAspect* rendering_aspect = m_quadEntity.AddAspect<RenderingAspect>();
    rendering_aspect->AddComponent<Core::RenderingNode*>("quad_rn", _DRAWSPACE_NEW_(Core::RenderingNode, Core::RenderingNode));
    rendering_aspect->AddComponent<bool>("draw", false);

    RenderingNode* quad_node = rendering_aspect->GetComponent<Core::RenderingNode*>( "quad_rn" )->getPurpose();

    quad_node->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    quad_node->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader(console_shader_vs, false ) ) );
    quad_node->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader(console_shader_ps, false ) ) );

    p_res_system->LoadShader(quad_node->GetFx()->GetShader(0), 0);
    p_res_system->LoadShader(quad_node->GetFx()->GetShader(1), 1);

    quad_node->AddShaderParameter( 1, "color", 0 );
    quad_node->SetShaderRealVector( "color", Vector( 0.05, 0.05, 0.85, 0.55 ) );

    RenderStatesSet quadpass_rss;

    quadpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    
    quadpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    quadpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    quadpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    quadpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha" ) );
    quadpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );


    quadpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    quadpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );


    quad_node->GetFx()->SetRenderStates( quadpass_rss );

    quad_node->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    rendering_aspect->AddImplementation(&m_quadRender, NULL);


    TransformAspect* transform_aspect = m_quadEntity.AddAspect<TransformAspect>();

    //transform_aspect->SetImplementation( &m_quadTransformer );
    transform_aspect->AddImplementation(0, &m_quadTransformer);

    
    transform_aspect->AddComponent<Matrix>( "quad_scaling" );
    transform_aspect->GetComponent<Matrix>( "quad_scaling" )->getPurpose().Scale( 1.0, 1.0, 1.0 );
    

    transform_aspect->AddComponent<Matrix>( "quad_pos" );
    transform_aspect->GetComponent<Matrix>( "quad_pos" )->getPurpose().Translation( 0.0, -0.5, -1.0 );

    // shaders quad charges dans le plugin graphique (QuadRenderingAspectImpl::Init()), on a plus besoin des data chargees des shaders : les liberer/retirer du resources system
    p_res_system->ReleaseShaderAsset(console_shader_vs);
    p_res_system->ReleaseShaderAsset(console_shader_ps);
}


void MainService::execute_lua_run_cbs( void )
{
    for( auto it = m_run_lua_callbacks.begin(); it != m_run_lua_callbacks.end(); ++it )
    {
        LuaContext::getInstance()->CallLuaFunc( it->second );
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

void MainService::RegisterKeyPressCallback( const dsstring& p_id, int p_regindex )
{
    m_keypress_lua_callbacks[p_id] = p_regindex;
}

int MainService::UnregisterKeyPressCallback( const dsstring& p_id )
{
    int index = -1;
    if( m_keypress_lua_callbacks.count( p_id ) )
    {
        index = m_keypress_lua_callbacks[p_id];
        m_keypress_lua_callbacks.erase( p_id );
    }
    return index;
}

void MainService::RegisterEndKeyPressCallback( const dsstring& p_id, int p_regindex )
{
    m_endkeypress_lua_callbacks[p_id] = p_regindex;
}

int MainService::UnregisterEndKeyPressCallback( const dsstring& p_id )
{
    int index = -1;
    if( m_endkeypress_lua_callbacks.count( p_id ) )
    {
        index = m_endkeypress_lua_callbacks[p_id];
        m_endkeypress_lua_callbacks.erase( p_id );
    }
    return index;
}

void MainService::RegisterOnCharCallback( const dsstring& p_id, int p_regindex )
{
    m_onchar_lua_callbacks[p_id] = p_regindex;
}

int MainService::UnregisterOnCharCallback( const dsstring& p_id )
{
    int index = -1;
    if( m_onchar_lua_callbacks.count( p_id ) )
    {
        index = m_onchar_lua_callbacks[p_id];
        m_onchar_lua_callbacks.erase( p_id );
    }
    return index;
}

void MainService::RegisterMouseMoveCallback( const dsstring& p_id, int p_regindex )
{
    m_mousemove_lua_callbacks[p_id] = p_regindex;
}

int MainService::UnregisterMouseMoveCallback( const dsstring& p_id )
{
    int index = -1;
    if( m_mousemove_lua_callbacks.count( p_id ) )
    {
        index = m_mousemove_lua_callbacks[p_id];
        m_mousemove_lua_callbacks.erase( p_id );
    }
    return index;
}

void MainService::RegisterMouseLeftButtonDownCallback( const dsstring& p_id, int p_regindex )
{
    m_mouseleftbuttondown_lua_callbacks[p_id] = p_regindex;
}

int MainService::UnregisterMouseLeftButtonDownCallback( const dsstring& p_id )
{
    int index = -1;
    if( m_mouseleftbuttondown_lua_callbacks.count( p_id ) )
    {
        index = m_mouseleftbuttondown_lua_callbacks[p_id];
        m_mouseleftbuttondown_lua_callbacks.erase( p_id );
    }
    return index;
}

void MainService::RegisterMouseLeftButtonUpCallback( const dsstring& p_id, int p_regindex )
{
    m_mouseleftbuttonup_lua_callbacks[p_id] = p_regindex;
}

int MainService::UnregisterMouseLeftButtonUpCallback( const dsstring& p_id )
{
    int index = -1;
    if( m_mouseleftbuttonup_lua_callbacks.count( p_id ) )
    {
        index = m_mouseleftbuttonup_lua_callbacks[p_id];
        m_mouseleftbuttonup_lua_callbacks.erase( p_id );
    }
    return index;
}

void MainService::RegisterMouseRightButtonDownCallback( const dsstring& p_id, int p_regindex )
{
    m_mouserightbuttondown_lua_callbacks[p_id] = p_regindex;
}

int MainService::UnregisterMouseRightButtonDownCallback( const dsstring& p_id )
{
    int index = -1;
    if( m_mouserightbuttondown_lua_callbacks.count( p_id ) )
    {
        index = m_mouserightbuttondown_lua_callbacks[p_id];
        m_mouserightbuttondown_lua_callbacks.erase( p_id );
    }
    return index;
}

void MainService::RegisterMouseRightButtonUpCallback( const dsstring& p_id, int p_regindex )
{
    m_mouserightbuttonup_lua_callbacks[p_id] = p_regindex;
}

int MainService::UnregisterMouseRightButtonUpCallback( const dsstring& p_id )
{
    int index = -1;
    if( m_mouserightbuttonup_lua_callbacks.count( p_id ) )
    {
        index = m_mouserightbuttonup_lua_callbacks[p_id];
        m_mouserightbuttonup_lua_callbacks.erase( p_id );
    }
    return index;
}


void MainService::RegisterGuiPushButtonClickedCallback( const dsstring& p_id, int p_regindex )
{
    m_guipushbuttonclicked_lua_callbacks[p_id] = p_regindex;
}

int MainService::UnregisterGuiPushButtonClickedCallback( const dsstring& p_id )
{
    int index = -1;
    if( m_guipushbuttonclicked_lua_callbacks.count( p_id ) )
    {
        index = m_guipushbuttonclicked_lua_callbacks[p_id];
        m_guipushbuttonclicked_lua_callbacks.erase( p_id );
    }
    return index;
}

void MainService::RegisterAnimationEventCallback(const dsstring& p_id, int p_regindex)
{
	m_animationevent_lua_callbacks[p_id] = p_regindex;
}

int MainService::UnregisterAnimationEventCallback(const dsstring& p_id)
{
	int index = -1;
	if (m_animationevent_lua_callbacks.count(p_id))
	{
		index = m_animationevent_lua_callbacks[p_id];
		m_animationevent_lua_callbacks.erase(p_id);
	}
	return index;
}

void MainService::RegisterResourceEventCallback(const dsstring& p_id, int p_regindex)
{
    m_resourceevent_lua_callbacks[p_id] = p_regindex;
}

int MainService::UnregisterResourceEventCallback(const dsstring& p_id)
{
    int index = -1;
    if (m_resourceevent_lua_callbacks.count(p_id))
    {
        index = m_resourceevent_lua_callbacks[p_id];
        m_resourceevent_lua_callbacks.erase(p_id);
    }
    return index;
}

DrawSpace::Systems::Hub* MainService::GetHub(void)
{
    return &m_systemsHub;
}

void MainService::RequestClose( void )
{
    (*m_closeapp_cb)( 0 );
}

void MainService::RequestClearConsole( void )
{
    m_console_texts.clear();
    m_console_texts.push_back( console_welcome );
    m_console_current_line = 0;
}

void MainService::RequestConsolePrint( const dsstring& p_msg )
{
    print_console_line( p_msg );
}

int MainService::RequestLuaFileExec(const dsstring& p_path)
{
	int status = LuaContext::getInstance()->ExecuteFromFile(p_path);
	if (-2 == status)
	{
		dsstring lua_err = LuaContext::getInstance()->GetLastError();

	    // erreur dans le script... on est potentiellement dans un etat merdique (operations du script pas menees jusqu'au bout puisque l'interpreteur n'est pas alle au bout)
	    // on prefere arreter toute l'appli...
	    _DSEXCEPTION("Error in executed script " + p_path + " : " + lua_err);
	}
	else if (-1 == status)
	{
		dsstring msg = "cannot open lua script file : " + p_path;
		print_console_line(msg);
	}

	return status;
}

void MainService::RequestMemAllocDump( void )
{
    MemAlloc::GetInstance()->DumpContent();
}

void MainService::RequestGuiDisplay( bool p_display )
{
    m_systemsHub.EnableGUI( p_display );
}

void MainService::RequestMouseCursorDisplayState( bool p_display )
{
    (*m_mousevisible_cb)( p_display );
}

void MainService::RequestMouseCursorCircularMode( bool p_state )
{
    set_mouse_circular_mode( p_state );
}

void MainService::RequestLuaStackReset()
{
    m_request_lua_reset = true;
}

void MainService::buil_lua_prerequisites( void )
{
    /// paths resources par defaut....
    Shader::EnableShadersDescrInFinalPath( true );
    Shader::SetRootPath( "console_data/shaders_bank" );
    LuaContext::getInstance()->SetRootPath( "lua_commons" );
    //File::MountVirtualFS( "test_data.bank" );


    m_console_ready = false;

    if( -1 == LuaContext::getInstance()->ExecuteFromFile( "gameroom.lua" ) )
    {
        _DSEXCEPTION( "Unable to open gameroom.lua" );
    }
   

    // build la console lua

    DrawSpace::EntityGraph::EntityNode& root_entity_node = m_entitygraphs["eg"]->GetEntityNode( "root" );
    DrawSpace::Core::Entity* root_entity = root_entity_node.GetEntity();

    RenderingAspect* rendering_aspect = root_entity->GetAspect<RenderingAspect>();

    rendering_aspect->AddImplementation( &m_textRender, NULL );
    
    rendering_aspect->AddComponent<std::vector<StringRenderingAspectImpl::TextDisplay>>( "console_lines" );

    m_console_ready = true;

    // ajout du quad console a la scene
    m_quadEntityNode = root_entity_node.AddChild( &m_quadEntity );
    
    m_rendergraphs["rg"]->GetRenderGraph().PushSignal_UpdatedRenderingQueues();
}

void MainService::RequestSignalRenderSceneBegin( const dsstring& p_entitygraph_id )
{
    if( m_entitygraphs.count( p_entitygraph_id ) )
    {
        m_entitygraphs[p_entitygraph_id]->GetEntityGraph().PushSignal_RenderSceneBegin();
    }
    else
    {
        _DSEXCEPTION( "Unknown entitygraph id" );
    }
}

void MainService::RequestSignalRenderSceneEnd( const dsstring& p_entitygraph_id )
{
    if( m_entitygraphs.count( p_entitygraph_id ) )
    {
        m_entitygraphs[p_entitygraph_id]->GetEntityGraph().PushSignal_RenderSceneEnd();
    }
    else
    {
        _DSEXCEPTION( "Unknown entitygraph id" );
    }
}

void MainService::RequestLog(int p_level, const dsstring& p_log)
{
    dsstring final_log = "LUALUALUALUALUALUALUALUALUALUALUALOG : " + p_log;
    switch( p_level )
    {
        default:
        case 0:
            _DSTRACE(logger, final_log);
            break;

        case 1:
            _DSDEBUG(logger, final_log);
            break;

        case 2:
            _DSWARN(logger, final_log);
            break;

        case 3:
            _DSERROR(logger, final_log);
            break;

        case 4:
            _DSFATAL(logger, final_log);
            break;
    }    
}

DrawSpace::Logger::Sink& MainService::RequestLogger(void) const
{
    return logger;
}

void MainService::RequestReleaseAssets(void)
{
    m_systemsHub.ReleaseAssets();
}

void MainService::RegisterNewModule(DrawSpace::Interface::Module::Root* p_mod_root)
{
    if(0 == m_loaded_modules.count( p_mod_root ) )
    {
        m_loaded_modules.insert( p_mod_root);
    }
    else
    {
        // exception
    }
}

void MainService::UnregisterNewModule(DrawSpace::Interface::Module::Root* p_mod_root)
{
    if (m_loaded_modules.count(p_mod_root))
    {
        m_loaded_modules.erase( p_mod_root );
    }
    else
    {
        // exception
    }
}

void MainService::ActivateResourcesSystem(const dsstring& p_context)
{
    m_systemsHub.GetSystem<DrawSpace::Systems::ResourcesSystem>("ResourcesSystem").Activate(p_context);
}

void MainService::DeactivateResourcesSystem(void)
{
    m_systemsHub.GetSystem<DrawSpace::Systems::ResourcesSystem>("ResourcesSystem").Deactivate();
}

DrawSpace::Logger::Configuration* MainService::GetLogConf(void) const
{
    return m_logconf;
}