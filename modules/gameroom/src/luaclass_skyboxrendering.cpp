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

#include "luacontext.h"
#include "luaclass_skyboxrendering.h"
#include "luaclass_module.h"
#include "luaclass_renderpassnodegraph.h"
#include "luaclass_renderconfig.h"
#include "luaclass_rendercontext.h"
#include "luaclass_texturesset.h"
#include "luaclass_fxparams.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;


const char LuaClass_SkyboxRendering::className[] = "SkyboxRendering";
const Luna<LuaClass_SkyboxRendering>::RegType LuaClass_SkyboxRendering::methods[] =
{
    { "instanciate_renderingimpl", &LuaClass_SkyboxRendering::LUA_instanciateRenderingImpl },
    { "trash_renderingimpl", &LuaClass_SkyboxRendering::LUA_trashRenderingImpl },
    { "attach_toentity", &LuaClass_SkyboxRendering::LUA_attachtoentity },
    { "detach_fromentity", &LuaClass_SkyboxRendering::LUA_detachfromentity },
    { "configure", &LuaClass_SkyboxRendering::LUA_configure },
    { "release", &LuaClass_SkyboxRendering::LUA_release },
    { "register_to_rendering", &LuaClass_SkyboxRendering::LUA_registertorendering },
    { "unregister_from_rendering", &LuaClass_SkyboxRendering::LUA_unregisterfromrendering },
	{ 0, 0 }
};

LuaClass_SkyboxRendering::LuaClass_SkyboxRendering( lua_State* p_L ):
m_skyboxRender( NULL )
{
}

LuaClass_SkyboxRendering::~LuaClass_SkyboxRendering( void )
{
}

int LuaClass_SkyboxRendering::LUA_instanciateRenderingImpl( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "SkyboxRendering::instanciate_renderingimpl : argument(s) missing" );
	}

    LuaClass_Module* lua_mod = Luna<LuaClass_Module>::check( p_L, 1 );

    m_skyboxRender = lua_mod->GetModuleRoot()->InstanciateRenderingAspectImpls( "skyboxRender" );

    return 0;
}

int LuaClass_SkyboxRendering::LUA_trashRenderingImpl( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "SkyboxRendering::trash_renderingimpl : argument(s) missing" );
	}

    LuaClass_Module* lua_mod = Luna<LuaClass_Module>::check( p_L, 1 );

    lua_mod->GetModuleRoot()->TrashRenderingAspectImpls( m_skyboxRender );

    return 0;
}

int LuaClass_SkyboxRendering::LUA_attachtoentity( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "SkyboxRendering::attach_toentity : argument(s) missing" );
	}

    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 1 );

    DrawSpace::Core::Entity& entity = lua_ent->GetEntity();
    RenderingAspect* rendering_aspect = entity.GetAspect<RenderingAspect>();

    if( NULL == rendering_aspect )
    {
        LUA_ERROR( "SkyboxRendering::attach_toentity : entity has no rendering aspect!" );
    }

    m_entity_rendering_aspect = rendering_aspect;
    m_entity = &entity;

    m_entity_rendering_aspect->AddImplementation( m_skyboxRender );

    return 0;
}

int LuaClass_SkyboxRendering::LUA_detachfromentity( lua_State* p_L )
{
    if( NULL == m_entity )
    {
        LUA_ERROR( "SkyboxRendering::detach_fromentity : argument(s) missing" );
    }

    LUA_TRY
    {
        m_entity_rendering_aspect->RemoveImplementation( m_skyboxRender );

    } LUA_CATCH;

    m_entity_rendering_aspect = NULL;
    m_entity = NULL;

    return 0;
}

int LuaClass_SkyboxRendering::LUA_configure( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "SkyboxRendering::configure : argument(s) missing" );
	}
    LuaClass_RenderConfig* lua_renderconfig = Luna<LuaClass_RenderConfig>::check( p_L, 1 );

    if( m_entity_rendering_aspect )
    {
        LUA_TRY
        {
            int rc_list_size = lua_renderconfig->GetRenderContextListSize();

            ////////////// noms des passes

            std::vector<dsstring> skybox_passes;            
            for( int i = 0; i < rc_list_size; i++ )
            {
                LuaClass_RenderContext* render_context = lua_renderconfig->GetRenderContext( i );

                dsstring pass_name = render_context->GetPassName();
                
                skybox_passes.push_back( pass_name );
            }
            m_entity_rendering_aspect->AddComponent<std::vector<dsstring>>( "skybox_passes", skybox_passes );
            
            ///////////////// jeux de textures pour chaque passes

            for( int i = 0; i < rc_list_size; i++ )
            {
                LuaClass_RenderContext* render_context = lua_renderconfig->GetRenderContext( i );
                dsstring pass_name = render_context->GetPassName();

                int textures_set_size = render_context->GetTexturesSetListSize();
                if( textures_set_size != 6 )
                {
                    LUA_ERROR( "SkyboxRendering::configure : textures set size must have 6 entries" );
                    // todo : cleanup_resources
                }

                ////////////// 6 jeux de 32 textures stages
                std::array<std::array<Texture*,RenderingNode::NbMaxTextures>,6> skybox_textures = { NULL };

                for( int texture_face_index = 0; texture_face_index < textures_set_size; texture_face_index++ )
                {
                    LuaClass_TexturesSet* txts_set = render_context->GetTexturesSet( texture_face_index );
                                        
                    for( int texture_stage_index = 0; texture_stage_index < RenderingNode::NbMaxTextures; texture_stage_index++ )
                    {
                        dsstring texture_name = txts_set->GetTextureFile( texture_stage_index );
                        if( texture_name != "" )
                        {
                            Texture* texture = _DRAWSPACE_NEW_( Texture, Texture( texture_name ) );

                            bool status = texture->LoadFromFile();

                            status = texture->LoadFromFile();
                            if( !status )
                            {                                
                                // todo : cleanup_resources
                                LUA_ERROR( "MesheRenderingAspectImpl::configure : texture loading operation failed" );
                            }
                            else
                            {
                                skybox_textures[texture_face_index][texture_stage_index] = texture;
                            }
                        }                        
                    }                    
                }

                dsstring component_name = "skybox_textures/" + pass_name;

                m_entity_rendering_aspect->AddComponent<std::array<std::array<Texture*,RenderingNode::NbMaxTextures>,6>>( component_name, skybox_textures );
            }

            ////////////////// fx pour chaque passes

            for( int i = 0; i < rc_list_size; i++ )
            {
                LuaClass_RenderContext* render_context = lua_renderconfig->GetRenderContext( i );
                dsstring pass_name = render_context->GetPassName();

                // pour les skybox, on a besoin que d'un seul fx....
                if( render_context->GetFxParamsListSize() < 1 )
                {
                    LUA_ERROR( "SkyboxRendering::configure : missing fx parameters description" );
                    // todo : cleanup_resources                    
                }
                LuaClass_FxParams* fx_params = render_context->GetFxParams( 0 );

                Fx* fx = _DRAWSPACE_NEW_( Fx, Fx );
              
                fx->SetRenderStates( fx_params->GetRenderStatesSet() );

                for( size_t j = 0; j < fx_params->GetNbShaderFiles(); j++ )
                {
                    std::pair<dsstring,bool> shader_file_infos = fx_params->GetShaderFile( j );
                    Shader* shader = _DRAWSPACE_NEW_( Shader, Shader( shader_file_infos.first, shader_file_infos.second ) );

                    bool status = shader->LoadFromFile();

                    if( !status )
                    {
                        // todo : cleanup_resources
                        LUA_ERROR( "MesheRenderingAspectImpl::configure : shader loading operation failed" );
                    }
                    else
                    {
                        fx->AddShader( shader );
                    }
                }

                dsstring component_name = "skybox_fx/" + pass_name;

                m_entity_rendering_aspect->AddComponent<Fx*>( component_name, fx );
            }

            //////////////// parametres de shaders
            for( int i = 0; i < rc_list_size; i++ )
            {
                std::vector<std::pair<dsstring, RenderingNode::ShadersParams>> skybox_texturepass_shaders_params;

                LuaClass_RenderContext* render_context = lua_renderconfig->GetRenderContext( i );
                dsstring pass_name = render_context->GetPassName();

                for( int j = 0; j < render_context->GetShadersParamsListSize(); j++ )
                {
                    LuaClass_RenderContext::NamedShaderParam param = render_context->GetNamedShaderParam( j );
                    skybox_texturepass_shaders_params.push_back( param );
                }

                dsstring component_name = "skybox_shaders_params/" + pass_name;

                m_entity_rendering_aspect->AddComponent<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>( component_name, skybox_texturepass_shaders_params );
            }

            ///////////////// rendering order

            for( int i = 0; i < rc_list_size; i++ )
            {
                LuaClass_RenderContext* render_context = lua_renderconfig->GetRenderContext( i );
                dsstring pass_name = render_context->GetPassName();

                dsstring component_name = "skybox_ro/" + pass_name;
                m_entity_rendering_aspect->AddComponent<int>( component_name, render_context->GetRenderingOrder() );
            }

        } LUA_CATCH; 
    }
    else
    {
        LUA_ERROR( "SkyboxRendering::configure : not attached to an entity" );
    }

    return 0;
}

void LuaClass_SkyboxRendering::cleanup_resources( lua_State* p_L )
{
    if( m_entity_rendering_aspect )
    {
        std::vector<dsstring> passes_list = m_entity_rendering_aspect->GetComponent<std::vector<dsstring>>( "skybox_passes" )->getPurpose();

        int rc_list_size = passes_list.size();

        //////////////// textures
        for( int i = 0; i < rc_list_size; i++ )
        {
            dsstring pass_name = passes_list[i];            
            dsstring component_name = "skybox_textures/" + pass_name;

            std::array<std::array<Texture*,RenderingNode::NbMaxTextures>,6> skybox_textures = m_entity_rendering_aspect->GetComponent<std::array<std::array<Texture*,RenderingNode::NbMaxTextures>,6>>( component_name )->getPurpose();

            for( size_t texture_face_index = 0; texture_face_index < skybox_textures.size(); texture_face_index++ )
            {
                std::array<Texture*,RenderingNode::NbMaxTextures> texture_set = skybox_textures[texture_face_index];
            
                for( size_t texture_stage_index = 0; texture_stage_index < texture_set.size(); texture_stage_index++ )
                {
                    Texture* texture = texture_set[texture_stage_index];
                    if( texture )
                    {
                        _DRAWSPACE_DELETE_( texture );
                    }
                }
            }

            LUA_TRY
            {
                m_entity_rendering_aspect->RemoveComponent<std::array<std::array<Texture*,RenderingNode::NbMaxTextures>,6>>( component_name );

            } LUA_CATCH;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        //////////////// fx
        for( int i = 0; i < rc_list_size; i++ )
        {
            dsstring pass_name = passes_list[i];            
            dsstring component_name = "skybox_fx/" + pass_name;

            Fx* fx = m_entity_rendering_aspect->GetComponent<Fx*>( component_name )->getPurpose();

            for( int j = 0; j < fx->GetShadersListSize(); j++ )
            {
                Shader* shader = fx->GetShader( j );
                _DRAWSPACE_DELETE_( shader );
            }
            _DRAWSPACE_DELETE_( fx );

            LUA_TRY
            {
                m_entity_rendering_aspect->RemoveComponent<Fx*>( component_name );

            } LUA_CATCH;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        //////////////// args shaders
        for( int i = 0; i < rc_list_size; i++ )
        {
            dsstring pass_name = passes_list[i];            
            dsstring component_name = "skybox_shaders_params/" + pass_name;

            m_entity_rendering_aspect->RemoveComponent<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>( component_name );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        //////////////// rendering orders
        for( int i = 0; i < rc_list_size; i++ )
        {
            dsstring pass_name = passes_list[i];            
            dsstring component_name = "skybox_ro/" + pass_name;

            m_entity_rendering_aspect->RemoveComponent<int>( component_name );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////

        LUA_TRY
        {
            m_entity_rendering_aspect->RemoveComponent<std::vector<dsstring>>( "skybox_passes" );

        } LUA_CATCH;

    }
    else
    {
        LUA_ERROR( "SkyboxRendering::cleanup_resources : no rendering aspect" );
    }
}

int LuaClass_SkyboxRendering::LUA_release( lua_State* p_L )
{
    cleanup_resources( p_L );
    return 0;
}

int LuaClass_SkyboxRendering::LUA_registertorendering( lua_State* p_L )
{
    if( NULL == m_skyboxRender )
    {
        LUA_ERROR( "SkyboxRendering::register_to_rendering : no rendering aspect impl created" );
    }

	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "SkyboxRendering::register_to_rendering : argument(s) missing" );
	}

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check( p_L, 1 );

    m_skyboxRender->RegisterToRendering( lua_rg->GetRenderGraph() );
    return 0;
}

int LuaClass_SkyboxRendering::LUA_unregisterfromrendering( lua_State* p_L )
{
    if( NULL == m_skyboxRender )
    {
        LUA_ERROR( "SkyboxRendering::unregister_from_rendering : no rendering aspect impl created" );
    }

	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "SkyboxRendering::unregister_from_rendering : argument(s) missing" );
	}

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check( p_L, 1 );

    m_skyboxRender->UnregisterFromRendering( lua_rg->GetRenderGraph() );
    return 0;
}

