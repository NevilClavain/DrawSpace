/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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

#include "luacontext.h"
#include "luaclass_rendering.h"
#include "luaclass_module.h"
#include "luaclass_renderpassnodegraph.h"
#include "luaclass_renderlayer.h"
#include "luaclass_texturesset.h"
#include "luaclass_fxparams.h"
#include "luaclass_timemanagerref.h"
#include "timemanager.h"

#include "mainservice.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;

const char LuaClass_Rendering::className[] = "Rendering";
const Luna<LuaClass_Rendering>::RegType LuaClass_Rendering::methods[] =
{    
    { "instanciate_renderingimpl", &LuaClass_Rendering::LUA_instanciateRenderingImpl },    
    { "trash_renderingimpl", &LuaClass_Rendering::LUA_trashRenderingImpl },
    { "attach_toentity", &LuaClass_Rendering::LUA_attachtoentity },
    { "detach_fromentity", &LuaClass_Rendering::LUA_detachfromentity },
    { "configure", &LuaClass_Rendering::LUA_configure },
    { "release", &LuaClass_Rendering::LUA_release },
    { "register_to_rendering", &LuaClass_Rendering::LUA_registertorendering },
    { "unregister_from_rendering", &LuaClass_Rendering::LUA_unregisterfromrendering },
    { "set_shaderrealvector", &LuaClass_Rendering::LUA_setshaderrealvector },
	{ "set_passforrendercontext", &LuaClass_Rendering::LUA_setPassForRenderContext },    
	{ 0, 0 }
};

LuaClass_Rendering::LuaClass_Rendering( lua_State* p_L ):
m_rendering_impl( NULL )
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Rendering::Rendering : argument(s) missing");
    }
    LuaClass_TimeManagerRef* lua_tmref = Luna<LuaClass_TimeManagerRef>::check(p_L, 1);
    m_tm = lua_tmref->GetTimeManager();
}

LuaClass_Rendering::~LuaClass_Rendering( void )
{
}

int LuaClass_Rendering::LUA_instanciateRenderingImpl( lua_State* p_L )
{
    
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{		
        LUA_ERROR( "Rendering::instanciate_renderingimpl : argument(s) missing" );
	}

    LuaClass_Module* lua_mod = Luna<LuaClass_Module>::check( p_L, 1 );
    dsstring implementation_id = luaL_checkstring(p_L, 2);

    // ici appel de MemAlloc::GetTotalSize()

    dsstring mod_name = lua_mod->GetModuleRoot()->GetModuleName();

    _DSDEBUG(MainService::GetInstance()->RequestLogger(), dsstring("BEGIN*******************Dumping ") << mod_name << dsstring( " mem allocs*******************"));
    lua_mod->GetModuleRoot()->GetMemAllocInstance()->DumpContent();
    _DSDEBUG(MainService::GetInstance()->RequestLogger(), dsstring("END**************************************************************************************"));

    size_t total = lua_mod->GetModuleRoot()->GetMemAllocInstance()->GetTotalSize();
    m_total_mem_allocs = total;

    m_rendering_impl = lua_mod->GetModuleRoot()->InstanciateRenderingAspectImpls( implementation_id );
    
    return 0;
}

int LuaClass_Rendering::LUA_trashRenderingImpl( lua_State* p_L )
{
    
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Rendering::trash_renderingimpl : argument(s) missing" );
	}

    LuaClass_Module* lua_mod = Luna<LuaClass_Module>::check( p_L, 1 );

    lua_mod->GetModuleRoot()->TrashRenderingAspectImpls( m_rendering_impl );

    // ici appel de MemAlloc::GetTotalSize() pour comparaison 

    dsstring mod_name = lua_mod->GetModuleRoot()->GetModuleName();

    _DSDEBUG(MainService::GetInstance()->RequestLogger(), dsstring("BEGIN*******************Dumping ") << mod_name << dsstring(" mem allocs*******************"));
    lua_mod->GetModuleRoot()->GetMemAllocInstance()->DumpContent();
    _DSDEBUG(MainService::GetInstance()->RequestLogger(), dsstring("END**************************************************************************************"));

    size_t total = lua_mod->GetModuleRoot()->GetMemAllocInstance()->GetTotalSize();

    if( total != m_total_mem_allocs )
    {
        dsstring log = dsstring( "RENDERING MODULE MEM LEAK DETECTED " ) + mod_name;
        MainService::GetInstance()->RequestLog(4, log); // 4 = FATAL
        _DSEXCEPTION( log );
    }
    
    return 0;
}

int LuaClass_Rendering::LUA_attachtoentity( lua_State* p_L )
{  
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Rendering::attach_toentity : argument(s) missing" );
	}

    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 1 );

    DrawSpace::Core::Entity& entity = lua_ent->GetEntity();
    RenderingAspect* rendering_aspect = entity.GetAspect<RenderingAspect>();

    if( NULL == rendering_aspect )
    {
        LUA_ERROR( "Rendering::attach_toentity : entity has no rendering aspect!" );
    }

    m_entity_rendering_aspect = rendering_aspect;
    m_entity = &entity;

    m_entity_rendering_aspect->AddImplementation( m_rendering_impl, m_tm);
    
    return 0;
}

int LuaClass_Rendering::LUA_detachfromentity( lua_State* p_L )
{
    
    if( NULL == m_entity )
    {
        LUA_ERROR( "Rendering::detach_fromentity : argument(s) missing" );
    }

    LUA_TRY
    {
        m_entity_rendering_aspect->RemoveImplementation( m_rendering_impl );

    } LUA_CATCH;

    m_entity_rendering_aspect = NULL;
    m_entity = NULL;
    
    return 0;
}

int LuaClass_Rendering::LUA_configure( lua_State* p_L )
{
    if( NULL == m_entity)
    {
        LUA_ERROR("Rendering::configure : no attached entity");
    }
    ResourcesAspect* resources_aspect = m_entity->GetAspect<ResourcesAspect>();
    if (!resources_aspect)
    {
        LUA_ERROR("Rendering::configure : attached entity has no resources aspect !");
    }

	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Rendering::configure : argument(s) missing" );
	}

    LuaClass_RenderLayer* lua_renderlayer = Luna<LuaClass_RenderLayer>::check(p_L, 1);

    if( m_entity_rendering_aspect )
    {
        LUA_TRY
        {
            std::vector<std::vector<dsstring>>                                                          passes_names_layers;
            std::vector<std::vector<std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>   layers_textures;
            std::vector<std::vector<Fx*>>                                                               layers_fx;
            std::vector<std::vector<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>    layers_shaders_params;
            std::vector<std::vector<int>>                                                               layers_ro;


            int rcfg_list_size = lua_renderlayer->GetRenderConfigListSize();
            for( int cfg_index = 0; cfg_index < rcfg_list_size; cfg_index++ )
            {
                LuaClass_RenderConfig::Data render_config = lua_renderlayer->GetRenderConfig(cfg_index);

                int rc_list_size = render_config.render_contexts.size();

                ////////////// noms des passes

                std::vector<dsstring> passes;
                for( int i = 0; i < rc_list_size; i++ )
                {
                    LuaClass_RenderContext::Data render_context = render_config.render_contexts[i];

					if (m_rcname_to_passes.end() != m_rcname_to_passes.find(render_context.rendercontexname))
					{
						dsstring pass_name = m_rcname_to_passes.at(render_context.rendercontexname);
						passes.push_back(pass_name);
					}
                }          
                passes_names_layers.push_back(passes);
  
                ///////////////// jeux de textures pour chaque passes

                std::vector<std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>> config_textures;
                for( int i = 0; i < rc_list_size; i++ )
                {
                    LuaClass_RenderContext::Data render_context = render_config.render_contexts[i];
                    //dsstring pass_name = render_context.passname;

                    int textures_set_size = render_context.textures_sets.size();

                    ////////////// les N jeux de 32 textures stages
                    std::vector<std::array<Texture*,RenderingNode::NbMaxTextures>> textures;

                    for( int texture_face_index = 0; texture_face_index < textures_set_size; texture_face_index++ )
                    {
                        std::array<Texture*, RenderingNode::NbMaxTextures> textures_set = { NULL };

                        LuaClass_TexturesSet::Data txts_set = render_context.textures_sets[texture_face_index];
                                        
                        for( int texture_stage_index = 0; texture_stage_index < RenderingNode::NbMaxTextures; texture_stage_index++ )
                        {
                            dsstring texture_name = txts_set.textures[texture_stage_index];
                            if( texture_name != "" )
                            {
                                Texture* texture = _DRAWSPACE_NEW_( Texture, Texture( texture_name ) );
                                dsstring res_id = dsstring("texture_") + std::to_string((int)texture);
                                resources_aspect->AddComponent<std::tuple<Texture*, bool>>(res_id, std::make_tuple(texture, false));

                                textures_set[texture_stage_index] = texture;
                            }                        
                        }

                        textures.push_back( textures_set );
                    }

                    config_textures.push_back(textures);
                }                
                layers_textures.push_back(config_textures);
                
                ////////////////// fx pour chaque passes

                std::vector<Fx*> config_fxs;
                for( int i = 0; i < rc_list_size; i++ )
                {
                    LuaClass_RenderContext::Data render_context = render_config.render_contexts[i];
                    //dsstring pass_name = render_context.passname;

                    if( render_context.fxparams.size() < 1 )
                    {
                        cleanup_resources( p_L );
                        LUA_ERROR( "Rendering::configure : missing fx parameters description" );                                   
                    }
                    LuaClass_FxParams::Data fx_params = render_context.fxparams[0];

                    Fx* fx = _DRAWSPACE_NEW_( Fx, Fx );
              
                    fx->SetRenderStates( fx_params.rss );

                    for( size_t j = 0; j < fx_params.shaders.size(); j++ )
                    {
                        std::pair<dsstring,bool> shader_file_infos = fx_params.shaders[j];
                        Shader* shader = _DRAWSPACE_NEW_( Shader, Shader( shader_file_infos.first, shader_file_infos.second ) );
                        dsstring res_id = dsstring("shader_") + std::to_string((int)shader);
                        resources_aspect->AddComponent<std::tuple<Shader*, bool>>(res_id, std::make_tuple(shader, false));
                        fx->AddShader(shader);
                    }

                    config_fxs.push_back( fx );
                }
                layers_fx.push_back(config_fxs);

                //////////////// parametres de shaders
                std::vector<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>> config_shadersparams;
                for( int i = 0; i < rc_list_size; i++ )
                {
                    std::vector<std::pair<dsstring, RenderingNode::ShadersParams>> texturepass_shaders_params;

                    LuaClass_RenderContext::Data render_context = render_config.render_contexts[i];
                    //dsstring pass_name = render_context.passname;

                    for( size_t j = 0; j < render_context.shaders_params.size(); j++ )
                    {
                        LuaClass_RenderContext::NamedShaderParam param = render_context.shaders_params[j];
                        texturepass_shaders_params.push_back( param );
                    }

                    config_shadersparams.push_back(texturepass_shaders_params);
                }
                layers_shaders_params.push_back(config_shadersparams);

                ///////////////// rendering order

                std::vector<int> config_ros;
                for( int i = 0; i < rc_list_size; i++ )
                {
                    LuaClass_RenderContext::Data render_context = render_config.render_contexts[i];
                    //dsstring pass_name = render_context.passname;

                    config_ros.push_back(render_context.rendering_order);
                }
                layers_ro.push_back(config_ros);
            }

            m_entity_rendering_aspect->AddComponent<std::vector<std::vector<dsstring>>>("passes", passes_names_layers);
            m_entity_rendering_aspect->AddComponent<std::vector<std::vector<std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>>("layers_textures", layers_textures);
            m_entity_rendering_aspect->AddComponent<std::vector<std::vector<Fx*>>>("layers_fx", layers_fx);
            m_entity_rendering_aspect->AddComponent<std::vector<std::vector<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>>("layers_shaders_params", layers_shaders_params);
            m_entity_rendering_aspect->AddComponent<std::vector<std::vector<int>>>("layers_ro", layers_ro);

        } LUA_CATCH; 
    }
    else
    {
        LUA_ERROR( "Rendering::configure : not attached to an entity" );
    }
    
    return 0;
}

void LuaClass_Rendering::cleanup_resources( lua_State* p_L )
{  
    if (NULL == m_entity)
    {
        LUA_ERROR("Rendering::cleanup_resources : no attached entity");
    }
    ResourcesAspect* resources_aspect = m_entity->GetAspect<ResourcesAspect>();
    if (!resources_aspect)
    {
        LUA_ERROR("Rendering::cleanup_resources : attached entity has no resources aspect !");
    }

    if( m_entity_rendering_aspect )
    {        
        Component<std::vector<std::vector<std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>>* layers_textures_comp;
                
        layers_textures_comp = m_entity_rendering_aspect->GetComponent<std::vector<std::vector<std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>>("layers_textures");
        if( layers_textures_comp )
        {
            std::vector<std::vector<std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>> layers_textures = layers_textures_comp->getPurpose();
            for (auto& e1 : layers_textures)
            {
                for (auto& e2 : e1)
                {
                    for (auto& e3 : e2)
                    {
                        std::array<Texture*, RenderingNode::NbMaxTextures> texture_set = e3;

                        for (size_t texture_stage_index = 0; texture_stage_index < texture_set.size(); texture_stage_index++)
                        {
                            Texture* texture = texture_set[texture_stage_index];
                            if (texture)
                            {
                                dsstring id;
                                dsstring res_id = dsstring("texture_") + std::to_string((int)texture);
                                resources_aspect->RemoveComponent<std::tuple<Texture*, bool>>(res_id);

                                _DRAWSPACE_DELETE_(texture);
                            }
                        }
                    }
                }
            }

            m_entity_rendering_aspect->RemoveComponent<std::vector<std::vector<std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>>("layers_textures");
        }
       
        ///////////////////////////////////////////////////////////////////////////////////////////
        //////////////// fx

        Component<std::vector<std::vector<Fx*>>>* layers_fx_comp;

        layers_fx_comp = m_entity_rendering_aspect->GetComponent<std::vector<std::vector<Fx*>>>("layers_fx");
        if( layers_fx_comp )
        {
            std::vector<std::vector<Fx*>> layers_fx = layers_fx_comp->getPurpose();
            for( auto& e1 : layers_fx)
            {
                for( auto& e2 : e1 )
                {
                    Fx* fx = e2;
                    for (int j = 0; j < fx->GetShadersListSize(); j++)
                    {
                        Shader* shader = fx->GetShader(j);

                        dsstring id;
                        dsstring res_id = dsstring("shader_") + std::to_string((int)shader);
                        resources_aspect->RemoveComponent<std::tuple<Shader*, bool>>(res_id);

                        _DRAWSPACE_DELETE_(shader);
                    }
                    _DRAWSPACE_DELETE_(fx);
                }
            }
            m_entity_rendering_aspect->RemoveComponent<std::vector<std::vector<Fx*>>>("layers_fx");
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        //////////////// args shaders
     
        if(m_entity_rendering_aspect->GetComponent<std::vector<std::vector<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>>("layers_shaders_params"))
        {
            m_entity_rendering_aspect->RemoveComponent<std::vector<std::vector<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>>("layers_shaders_params");
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        //////////////// rendering orders

        if(m_entity_rendering_aspect->GetComponent<std::vector<std::vector<int>>>("layers_ro"))
        {
            m_entity_rendering_aspect->RemoveComponent<std::vector<std::vector<int>>>( "layers_ro" );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        //////////////// passes names

        if(m_entity_rendering_aspect->GetComponent< std::vector<std::vector<dsstring>>>("passes"))
        {
            m_entity_rendering_aspect->RemoveComponent<std::vector<std::vector<dsstring>>>("passes");
        }
    }
    else
    {
        LUA_ERROR( "Rendering::cleanup_resources : no rendering aspect" );
    }
    
}

int LuaClass_Rendering::LUA_release( lua_State* p_L )
{
    cleanup_resources( p_L );
    return 0;
}

int LuaClass_Rendering::LUA_registertorendering( lua_State* p_L )
{
    
    if( NULL == m_rendering_impl )
    {
        LUA_ERROR( "Rendering::register_to_rendering : no rendering aspect impl created" );
    }

	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Rendering::register_to_rendering : argument(s) missing" );
	}

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check( p_L, 1 );

    m_rendering_impl->RegisterToRendering( lua_rg->GetRenderGraph() );
    
    return 0;
}

int LuaClass_Rendering::LUA_unregisterfromrendering( lua_State* p_L )
{
    
    if( NULL == m_rendering_impl)
    {
        LUA_ERROR( "Rendering::unregister_from_rendering : no rendering aspect impl created" );
    }

	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Rendering::unregister_from_rendering : argument(s) missing" );
	}

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check( p_L, 1 );

    m_rendering_impl->UnregisterFromRendering( lua_rg->GetRenderGraph() );
    
    return 0;
}

int LuaClass_Rendering::LUA_setshaderrealvector( lua_State* p_L )
{   
	int argc = lua_gettop( p_L );
	if( argc < 7 )
	{		
        LUA_ERROR( "Rendering::set_shaderrealvector : argument(s) missing" );
	}

    int rendering_layer_index = luaL_checkinteger(p_L, 1);
    dsstring pass_id = luaL_checkstring( p_L, 2 );
    dsstring param_id = luaL_checkstring( p_L, 3 );
    
    dsreal valx = luaL_checknumber( p_L, 4 );
    dsreal valy = luaL_checknumber( p_L, 5 );
    dsreal valz = luaL_checknumber( p_L, 6 );
    dsreal valw = luaL_checknumber( p_L, 7 );
    
    LUA_TRY
    {
        std::vector<std::vector<dsstring>> passes_names_layers = m_entity_rendering_aspect->GetComponent<std::vector<std::vector<dsstring>>>("passes")->getPurpose();

        std::vector<dsstring> passes_names = passes_names_layers[rendering_layer_index];

        // find passe index
        size_t pass_index;
        bool pass_found = false;
        for( pass_index = 0; pass_index < passes_names.size(); pass_index++ )
        {
            if(passes_names[pass_index] == pass_id)
            {
                pass_found = true;
                break;
            }        
        }

        if( !pass_found )
        {
            LUA_ERROR("Rendering::set_shaderrealvector : unknown pass id");
        }

        std::vector<std::vector<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>> layers_shaders_params =
            m_entity_rendering_aspect->GetComponent< std::vector<std::vector<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>>("layers_shaders_params")->getPurpose();


        std::vector<std::pair<dsstring, RenderingNode::ShadersParams>> pass_shaders_params = layers_shaders_params[rendering_layer_index][pass_index];

        
        for( auto it = pass_shaders_params.begin(); it != pass_shaders_params.end(); ++it )
        {
            if( it->first == param_id )
            {
                it->second.vector = true;
                it->second.param_values[0] = valx;
                it->second.param_values[1] = valy;
                it->second.param_values[2] = valz;
                it->second.param_values[3] = valw;

                m_entity_rendering_aspect->GetComponent< std::vector<std::vector<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>>("layers_shaders_params")->getPurpose()[rendering_layer_index][pass_index] = pass_shaders_params;

                break;
            }
        }
        
    } LUA_CATCH;
    
    return 0;
}

int LuaClass_Rendering::LUA_setPassForRenderContext(lua_State* p_L)
{
	int argc = lua_gettop(p_L);
	if (argc < 2)
	{
		LUA_ERROR("Rendering::set_passforrendercontext : argument(s) missing");
	}

	dsstring rc_id = luaL_checkstring(p_L, 1);
	dsstring pass_id = luaL_checkstring(p_L, 2);

	if (m_rcname_to_passes.find(rc_id) == m_rcname_to_passes.end())
	{
		m_rcname_to_passes[rc_id] = pass_id;
	}
	else
	{
		LUA_ERROR("Rendering::set_passforrendercontext : rendercontext name already exists in rcname_to_passes table !");
	}

	return 0;
}
