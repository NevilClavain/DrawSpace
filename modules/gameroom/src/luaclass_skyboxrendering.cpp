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
    if( NULL == lua_mod )
    {
        LUA_ERROR( "SkyboxRendering::instanciate_renderingimpl : argument 1 must be of type LuaClass_Module" );
    }

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
    if( NULL == lua_mod )
    {
        LUA_ERROR( "SkyboxRendering::trash_renderingimpl : argument 1 must be of type LuaClass_Module" );
    }

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
    if( NULL == lua_ent )
    {
        LUA_ERROR( "SkyboxRendering::attach_toentity : argument 1 must be of type LuaClass_Entity" );
    }

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
    if( NULL == lua_renderconfig )
    {
        LUA_ERROR( "SkyboxRendering::configure : argument 1 must be of type LuaClass_RenderConfig" );
    }

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
            
            ////////////// jeu de 6 textures par slot pass

            for( int i = 0; i < rc_list_size; i++ )
            {
                LuaClass_RenderContext* render_context = lua_renderconfig->GetRenderContext( i );
                int textures_set_size = render_context->GetTexturesSetListSize();

                if( textures_set_size != 6 )
                {
                    LUA_ERROR( "SkyboxRendering::configure : textures set size must have 6 entries" );
                    // todo : cleanup_resources
                }

                std::vector<Texture*> skybox_textures;

                for( int j = 0; j < textures_set_size; j++ )
                {
                    LuaClass_TexturesSet* txts_set = render_context->GetTexturesSet( j );
                    
                    
                    for( int k = 0; k < RenderingNode::GetTextureListSize(); k++ )
                    {
                        dsstring texture_name = txts_set->GetTextureFile( k );
                        if( texture_name != "" )
                        {
                            Texture* texture = _DRAWSPACE_NEW_( Texture, Texture( texture_name ) );
                            // ici...

                            _asm nop
                        }                        
                    }                    
                }
            }

        } LUA_CATCH; 
    }
    else
    {
        LUA_ERROR( "SkyboxRendering::configure : not attached to an entity" );
    }

    return 0;
}

int LuaClass_SkyboxRendering::LUA_release( lua_State* p_L )
{
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
    if( NULL == lua_rg )
    {
        LUA_ERROR( "SkyboxRendering::register_to_rendering : argument 1 must be of type LuaClass_RenderPassNodeGraph" );
    }

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
    if( NULL == lua_rg )
    {
        LUA_ERROR( "SkyboxRendering::unregister_from_rendering : argument 1 must be of type LuaClass_RenderPassNodeGraph" );
    }

    m_skyboxRender->UnregisterFromRendering( lua_rg->GetRenderGraph() );
    return 0;
}

