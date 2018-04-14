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
#include "luaclass_mesherenderingaspectimpl.h"
#include "luaclass_renderassembly.h"
#include "luaclass_renderpassnodegraph.h"
#include "mainservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;


const char LuaClass_MesheRenderingAspectImpl::className[] = "MesheRenderingAspectImpl";
const Luna<LuaClass_MesheRenderingAspectImpl>::RegType LuaClass_MesheRenderingAspectImpl::methods[] =
{
    { "attach_toentity", &LuaClass_MesheRenderingAspectImpl::LUA_attachtoentity },
    { "detach_fromentity", &LuaClass_MesheRenderingAspectImpl::LUA_detachfromentity },
    { "configure", &LuaClass_MesheRenderingAspectImpl::LUA_configure },
    { "release", &LuaClass_MesheRenderingAspectImpl::LUA_release },
    { "register_to_rendering", &LuaClass_MesheRenderingAspectImpl::LUA_registertorendering },
    { "unregister_from_rendering", &LuaClass_MesheRenderingAspectImpl::LUA_unregisterfromrendering },
	{ 0, 0 }
};

LuaClass_MesheRenderingAspectImpl::LuaClass_MesheRenderingAspectImpl( lua_State* p_L ) :
m_meshe_render( NULL ),
m_entity( NULL )
{
    m_meshe.SetImporter( MainService::GetInstance()->GetMesheImport() );
}

LuaClass_MesheRenderingAspectImpl::~LuaClass_MesheRenderingAspectImpl( void )
{
}

int LuaClass_MesheRenderingAspectImpl::LUA_attachtoentity( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "MesheRenderingAspectImpl::attach_toentity : argument(s) missing" );
	}

    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 1 );
    if( NULL == lua_ent )
    {
        LUA_ERROR( "MesheRenderingAspectImpl::attach_toentity : argument 1 must be of type LuaClass_Entity" );
    }

    DrawSpace::Core::Entity& entity = lua_ent->GetEntity();
    RenderingAspect* rendering_aspect = entity.GetAspect<RenderingAspect>();

    if( NULL == rendering_aspect )
    {
        LUA_ERROR( "MesheRenderingAspectImpl::attach_toentity : entity has no rendering aspect!" );
    }

    m_entity_rendering_aspect = rendering_aspect;
    m_entity = &entity;

    m_meshe_render = _DRAWSPACE_NEW_( DrawSpace::AspectImplementations::MesheRenderingAspectImpl, DrawSpace::AspectImplementations::MesheRenderingAspectImpl );
    m_entity_rendering_aspect->AddImplementation( m_meshe_render );

    return 0;
}

int LuaClass_MesheRenderingAspectImpl::LUA_detachfromentity( lua_State* p_L )
{
    if( NULL == m_entity )
    {
        LUA_ERROR( "MesheRenderingAspectImpl::detach_fromentity : argument(s) missing" );
    }

    if( m_meshe_render )
    {
        _DRAWSPACE_DELETE_( m_meshe_render );
    }

    LUA_TRY
    {
        m_entity_rendering_aspect->RemoveImplementation( m_meshe_render );

    } LUA_CATCH; 

    m_entity_rendering_aspect = NULL;
    m_entity = NULL;

    return 0;
}


int LuaClass_MesheRenderingAspectImpl::LUA_configure( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 4 )
	{		
        LUA_ERROR( "MesheRenderingAspectImpl::configure : argument(s) missing" );
	}

    /*
    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 1 );
    if( NULL == lua_ent )
    {
        LUA_ERROR( "MesheRenderingAspectImpl::configure : argument 1 must be of type LuaClass_Entity" );
    }
    */

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    LuaClass_RenderAssembly* lua_renderassembly = Luna<LuaClass_RenderAssembly>::check( p_L, 2 );
    if( NULL == lua_renderassembly )
    {
        LUA_ERROR( "MesheRenderingAspectImpl::configure : argument 1 must be of type LuaClass_RenderAssembly" );
    }

    dsstring meshe_path = luaL_checkstring( p_L, 3 );
    int meshe_index = luaL_checkint( p_L, 4 );

    //DrawSpace::Core::Entity& entity = lua_ent->GetEntity();

    // recupere l'aspect rendu s'il existe pour cette entitee
    //RenderingAspect* rendering_aspect = entity.GetAspect<RenderingAspect>();
    //if( rendering_aspect )

    if( m_entity_rendering_aspect )
    {
        
        //m_meshe_render = _DRAWSPACE_NEW_( DrawSpace::AspectImplementations::MesheRenderingAspectImpl, DrawSpace::AspectImplementations::MesheRenderingAspectImpl );
        //m_entity_rendering_aspect->AddImplementation( m_meshe_render );
        
        LUA_TRY
        {
            m_entity_rendering_aspect->AddComponent<MesheRenderingAspectImpl::PassSlot>( pass_id, pass_id );
            RenderingNode* rnode = m_entity_rendering_aspect->GetComponent<MesheRenderingAspectImpl::PassSlot>( pass_id )->getPurpose().GetRenderingNode();


            //m_entity_rendering_aspect = rendering_aspect;

            // ici completer le passSlot avec les infos du RenderAssembly

            ///////////////////////// les shaders
            size_t nb_shaders = lua_renderassembly->GetNbShaderFiles();

            for( size_t i = 0; i < nb_shaders; i++ )
            {
                std::pair<dsstring,bool> shader_infos = lua_renderassembly->GetShaderFile( i );

                dsstring shader_path = shader_infos.first;
                bool is_compiled = shader_infos.second;

                bool status;

                Shader* shader = _DRAWSPACE_NEW_( Shader, Shader );
                status = shader->LoadFromFile( shader_path, is_compiled );

                if( !status )
                {
                    // clean tout ce qui a deja ete charge...
                    cleanup_resources( p_L, pass_id );
                    LUA_ERROR( "MesheRenderingAspectImpl::configure : shader loading operation failed" );
                }
                else
                {
                    m_fx.AddShader( shader );                    
                }
            }

            ///////////////////////// les rendestates

            DrawSpace::Core::RenderStatesSet& rss = lua_renderassembly->GetRenderStatesSet();
            m_fx.SetRenderStates( rss );

            ///////////////////////// les textures

            for( size_t i = 0; i < DrawSpace::Core::RenderingNode::NbMaxTextures; i++ )
            {
                dsstring texture_path = lua_renderassembly->GetTextureFile( i );
                if( texture_path != "" )
                {
                    bool status;
                    Texture* texture = _DRAWSPACE_NEW_( Texture, Texture( texture_path ) );
                    status = texture->LoadFromFile();
                    if( !status )
                    {
                        // clean tout ce qui a deja ete charge...
                        cleanup_resources( p_L, pass_id );
                        LUA_ERROR( "MesheRenderingAspectImpl::load_pass_viewportquad_resources : texture loading operation failed" );
                    }
                    else
                    {
                        rnode->SetTexture( texture, i );
                    }
                }
            }

            ///////

            // et pour finir, le meshe
            bool status = m_meshe.LoadFromFile( meshe_path, meshe_index );
            if( !status )
            {
                cleanup_resources( p_L, pass_id );
                LUA_ERROR( "MesheRenderingAspectImpl::configure : meshe loading operation failed" );
            }
            rnode->SetMeshe( &m_meshe );
            rnode->SetFx( &m_fx );

        } LUA_CATCH;

        //m_id = pass_id;
    }
    else
    {
        //m_entity_rendering_aspect = NULL;
        //LUA_ERROR( "MesheRenderingAspectImpl::configure : entity passed on arg has no rendering aspect" );
        LUA_ERROR( "MesheRenderingAspectImpl::configure : not attached to an entity" );
    }

    return 0;
}

void LuaClass_MesheRenderingAspectImpl::cleanup_resources( lua_State* p_L, const dsstring& p_id )
{
    if( m_entity_rendering_aspect )
    {
        RenderingNode* rnode = m_entity_rendering_aspect->GetComponent<MesheRenderingAspectImpl::PassSlot>( p_id )->getPurpose().GetRenderingNode();

        for( long i = 0; i < m_fx.GetShadersListSize(); i++ )
        {
            Shader* shader = m_fx.GetShader( i );
            _DRAWSPACE_DELETE_( shader );
        }
        m_fx.ClearShaders();

        for( long i = 0; i < rnode->GetTextureListSize(); i++ )
        {
            Texture* texture = rnode->GetTexture( i );
            if( texture )
            {
                _DRAWSPACE_DELETE_( texture );
                rnode->SetTexture( NULL, i );
            }
        }

        LUA_TRY
        {
            m_entity_rendering_aspect->RemoveComponent<MesheRenderingAspectImpl::PassSlot>( p_id );
            //m_entity_rendering_aspect->RemoveImplementation( m_meshe_render );

        } LUA_CATCH; 

        /*
        if( m_meshe_render )
        {
            _DRAWSPACE_DELETE_( m_meshe_render );
        }
        */
        //m_entity_rendering_aspect = NULL;
        //m_id = "";
    }
    else
    {
        LUA_ERROR( "MesheRenderingAspectImpl::release : no rendering aspect" );
    }
}

int LuaClass_MesheRenderingAspectImpl::LUA_release( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "MesheRenderingAspectImpl::release : argument(s) missing" );
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );

    cleanup_resources( p_L, pass_id );
    return 0;
}

int LuaClass_MesheRenderingAspectImpl::LUA_registertorendering( lua_State* p_L )
{
    if( NULL == m_meshe_render )
    {
        LUA_ERROR( "MesheRenderingAspectImpl::register_to_rendering : no meshe rendering aspect impl created" );
    }

	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "MesheRenderingAspectImpl::register_to_rendering : argument(s) missing" );
	}

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check( p_L, 1 );
    if( NULL == lua_rg )
    {
        LUA_ERROR( "MesheRenderingAspectImpl::register_to_rendering : argument 1 must be of type LuaClass_RenderPassNodeGraph" );
    }

    m_meshe_render->RegisterToRendering( lua_rg->GetRenderGraph() );
    return 0;
}

int LuaClass_MesheRenderingAspectImpl::LUA_unregisterfromrendering( lua_State* p_L )
{
    if( NULL == m_meshe_render )
    {
        LUA_ERROR( "MesheRenderingAspectImpl::unregister_from_rendering : no meshe rendering aspect impl created" );
    }

	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "MesheRenderingAspectImpl::unregister_from_rendering : argument(s) missing" );
	}

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check( p_L, 1 );
    if( NULL == lua_rg )
    {
        LUA_ERROR( "MesheRenderingAspectImpl::unregister_from_rendering : argument 1 must be of type LuaClass_RenderPassNodeGraph" );
    }

    m_meshe_render->UnregisterFromRendering( lua_rg->GetRenderGraph() );
    return 0;
}