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

#include "luaclass_renderpassnodegraph.h"
#include "luaclass_renderstatesset.h"
#include <drawspace_commons.h>
#include "mainservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaClass_RenderPassNodeGraph::className[] = "RenderPassNodeGraph";
const Luna<LuaClass_RenderPassNodeGraph>::RegType LuaClass_RenderPassNodeGraph::methods[] =
{
    { "create_rootpass", &LuaClass_RenderPassNodeGraph::LUA_createrootpass },
    { "create_childpass", &LuaClass_RenderPassNodeGraph::LUA_createchildpass },
    { "remove_pass", &LuaClass_RenderPassNodeGraph::LUA_removepass },
    { "set_pass_targetclearcolor", &LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearcolor },
    { "set_pass_targetclearstate", &LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearstate },
    { "set_pass_depthclearstate", &LuaClass_RenderPassNodeGraph::LUA_setpassdepthclearstate },
    { "create_pass_viewportquad", &LuaClass_RenderPassNodeGraph::LUA_createpassviewportquad },
    { "remove_pass_viewportquad", &LuaClass_RenderPassNodeGraph::LUA_removepassviewportquad },
    { "load_pass_viewportquad_shader", &LuaClass_RenderPassNodeGraph::LUA_loadpassviewportquadshader },
    { "unload_pass_viewportquad_shaders", &LuaClass_RenderPassNodeGraph::LUA_unloadpassviewportquadshaders },
    { "load_pass_viewportquad_texture", &LuaClass_RenderPassNodeGraph::LUA_loadpassviewportquadtexture },
    { "unload_pass_viewportquad_textures", &LuaClass_RenderPassNodeGraph::LUA_unloadpassviewportquadtextures },
    { "set_pass_viewportquad_renderstateset", &LuaClass_RenderPassNodeGraph::LUA_setpassviewportquadrenderstateset },
    { "update_renderingqueues", &LuaClass_RenderPassNodeGraph::LUA_updaterenderingqueues },
	{ 0, 0 }
};

LuaClass_RenderPassNodeGraph::LuaClass_RenderPassNodeGraph( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::RenderPassNodeGraph : argument(s) missing" );
		lua_error( p_L );		
	}

    dsstring id = luaL_checkstring( p_L, 1 );
    m_passes_render.SetRendergraph( &m_rendergraph );
    MainService::GetInstance()->RegisterRenderGraph( id, this );
}

LuaClass_RenderPassNodeGraph::~LuaClass_RenderPassNodeGraph( void )
{
}

DrawSpace::AspectImplementations::PassesRenderingAspectImpl& LuaClass_RenderPassNodeGraph::GetPassesRenderAspectImpl( void )
{
    return m_passes_render;
}

int LuaClass_RenderPassNodeGraph::LUA_createrootpass( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::create_rootpass : argument(s) missing" );
		lua_error( p_L );		
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    
    m_passes[pass_id].m_renderpassnode = m_rendergraph.CreateRoot( pass_id );

    // reglages par defaut
    m_passes[pass_id].m_renderpassnode.GetRenderingQueue()->EnableDepthClearing( false );
    m_passes[pass_id].m_renderpassnode.GetRenderingQueue()->EnableTargetClearing( true );
    m_passes[pass_id].m_renderpassnode.GetRenderingQueue()->SetTargetClearingColor( 26, 27, 180, 255 );   
    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_createchildpass( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::create_childpass : argument(s) missing" );
		lua_error( p_L );		
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    dsstring child_pass_id = luaL_checkstring( p_L, 2 );
    int target_stage = luaL_checkint( p_L, 3 );

    if( m_passes.count( pass_id ) )
    {
        m_passes[child_pass_id].m_renderpassnode = m_passes[pass_id].m_renderpassnode.CreateChild( child_pass_id, target_stage );

        // reglages par defaut
        m_passes[child_pass_id].m_renderpassnode.GetRenderingQueue()->EnableDepthClearing( true );
        m_passes[child_pass_id].m_renderpassnode.GetRenderingQueue()->EnableTargetClearing( true );
        m_passes[child_pass_id].m_renderpassnode.GetRenderingQueue()->SetTargetClearingColor( 26, 130, 18, 255 );   
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::set_pass_targetclearcolor : unknown pass id" );
        lua_error( p_L );	
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_removepass( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::remove_pass : argument(s) missing" );
		lua_error( p_L );		
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );

    if( m_passes.count( pass_id ) )
    {
        m_passes[pass_id].m_renderpassnode.Erase();
        m_passes.erase( pass_id );
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::remove_pass : unknown pass id" );
        lua_error( p_L );    
    }
    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearcolor( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 4 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::set_pass_targetclearcolor : argument(s) missing" );
		lua_error( p_L );		
	}

	dsstring pass_id = luaL_checkstring( p_L, 1 );
    int r = luaL_checkint( p_L, 2 );
    int g = luaL_checkint( p_L, 3 );
    int b = luaL_checkint( p_L, 4 );

    if( m_passes.count( pass_id ) )
    {
        m_passes[pass_id].m_renderpassnode.GetRenderingQueue()->SetTargetClearingColor( r, g, b, 255 );
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::set_pass_targetclearcolor : unknown pass id" );
        lua_error( p_L );	
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearstate( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::set_pass_targetclearstate : argument(s) missing" );
		lua_error( p_L );		
	}

	dsstring pass_id = luaL_checkstring( p_L, 1 );
    int state = luaL_checkint( p_L, 2 );

    if( m_passes.count( pass_id ) )
    {
        m_passes[pass_id].m_renderpassnode.GetRenderingQueue()->EnableTargetClearing( state );
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::set_pass_targetclearstate : unknown pass id" );
        lua_error( p_L );	
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_setpassdepthclearstate( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::set_pass_depthclearstate : argument(s) missing" );
		lua_error( p_L );		
	}

	dsstring pass_id = luaL_checkstring( p_L, 1 );
    int state = luaL_checkint( p_L, 2 );

    if( m_passes.count( pass_id ) )
    {
        m_passes[pass_id].m_renderpassnode.GetRenderingQueue()->EnableDepthClearing( state );
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::set_pass_depthclearstate : unknown pass id" );
        lua_error( p_L );	
    }
    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_createpassviewportquad( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::create_pass_viewportquad : argument(s) missing" );
		lua_error( p_L );		
	}

	dsstring pass_id = luaL_checkstring( p_L, 1 );

    dsreal zdepth = 0.0;
    if( argc > 1 )
    {
        zdepth = luaL_checknumber( p_L, 2 );
    }

    if( m_passes.count( pass_id ) )
    {
        m_passes[pass_id].m_renderpassnode.CreateViewportQuad( zdepth );
        m_passes[pass_id].m_renderpassnode.GetViewportQuad()->SetFx( &( m_passes[pass_id].m_fx ) );

        /*
        m_passes[pass_id].m_fx.AddShader( &m_passes[pass_id].m_v_shader );
        m_passes[pass_id].m_fx.AddShader( &m_passes[pass_id].m_p_shader );
        */
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::create_pass_viewportquad : unknown pass id" );
        lua_error( p_L );	
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_removepassviewportquad( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::remove_pass_viewportquad : argument(s) missing" );
		lua_error( p_L );		
	}

	dsstring pass_id = luaL_checkstring( p_L, 1 );

    if( m_passes.count( pass_id ) )
    {
        m_passes[pass_id].m_renderpassnode.RemoveViewportQuad();
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::remove_pass_viewportquad : unknown pass id" );
        lua_error( p_L );    
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_loadpassviewportquadshader( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::load_pass_viewportquad_shader : argument(s) missing" );
		lua_error( p_L );		
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    dsstring shader_path = luaL_checkstring( p_L, 2 );
    bool is_compiled = luaL_checkstring( p_L, 3 );

    if( m_passes.count( pass_id ) )
    {
        bool status;

        Shader* shader = _DRAWSPACE_NEW_( Shader, Shader );
        status = shader->LoadFromFile( shader_path, is_compiled );

        if( !status )
        {
            lua_pushstring( p_L, "RenderPassNodeGraph::load_pass_viewportquad_shader : shader loading operation failed" );
            lua_error( p_L );	        
        }
        else
        {
            m_passes[pass_id].m_fx.AddShader( shader );
            m_passes[pass_id].m_renderpassnode.SetRenderingQueueUpdateFlag();
        }
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::load_pass_viewportquad_shader : unknown pass id" );
        lua_error( p_L );	
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_unloadpassviewportquadshaders( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::unload_pass_viewportquad_shaders : argument(s) missing" );
		lua_error( p_L );		
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );


    if( m_passes.count( pass_id ) )
    {
        bool update_queue = false;
        for( long i = 0; i < m_passes[pass_id].m_fx.GetShadersListSize(); i++ )
        {
            Shader* shader = m_passes[pass_id].m_fx.GetShader( i );
            _DRAWSPACE_DELETE_( shader );

            update_queue = true;
        }
        m_passes[pass_id].m_fx.ClearShaders();

        if( update_queue )
        {
            m_passes[pass_id].m_renderpassnode.SetRenderingQueueUpdateFlag();
        }
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::unload_pass_viewportquad_shaders : unknown pass id" );
        lua_error( p_L );	   
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_loadpassviewportquadtexture( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::load_pass_viewportquad_texture : argument(s) missing" );
		lua_error( p_L );		
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    dsstring texture_path = luaL_checkstring( p_L, 2 );
    int stage = luaL_checkint( p_L, 3 );

    if( m_passes.count( pass_id ) )
    {
        bool status;
        ViewportQuad* vpq = m_passes[pass_id].m_renderpassnode.GetViewportQuad();

        if( vpq )
        {
            Texture* texture = _DRAWSPACE_NEW_( Texture, Texture( texture_path ) );
            status = texture->LoadFromFile();
            if( !status )
            {
                lua_pushstring( p_L, "RenderPassNodeGraph::load_pass_viewportquad_texture : texture loading operation failed" );
                lua_error( p_L );	        
            }
            else
            {
                m_passes[pass_id].m_renderpassnode.GetViewportQuad()->SetTexture( texture, stage );
                m_passes[pass_id].m_renderpassnode.SetRenderingQueueUpdateFlag();
            }
        }
        else
        {
            lua_pushstring( p_L, "RenderPassNodeGraph::load_pass_viewportquad_texture : no viewportquad created for this pass" );
            lua_error( p_L );        
        }        
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::load_pass_viewportquad_texture : unknown pass id" );
        lua_error( p_L );	
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_unloadpassviewportquadtextures( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::unload_pass_viewportquad_textures : argument(s) missing" );
		lua_error( p_L );		
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );

    if( m_passes.count( pass_id ) )
    {
        ViewportQuad* vpq = m_passes[pass_id].m_renderpassnode.GetViewportQuad();
        
        if( vpq )
        {
            bool update_queue = false;
            for( long i = 0; i < vpq->GetTextureListSize(); i++ )
            {
                Texture* texture = vpq->GetTexture( i );
                if( texture )
                {
                    _DRAWSPACE_DELETE_( texture );
                     vpq->SetTexture( NULL, i );
                     update_queue = true;
                }
            }

            if( update_queue )
            {
                m_passes[pass_id].m_renderpassnode.SetRenderingQueueUpdateFlag();
            }
        }
        else
        {
            lua_pushstring( p_L, "RenderPassNodeGraph::unload_pass_viewportquad_textures : no viewportquad created for this pass" );
            lua_error( p_L );	        
        }
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::unload_pass_viewportquad_textures : unknown pass id" );
        lua_error( p_L );	
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_setpassviewportquadrenderstateset( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::set_pass_viewportquad_renderstateset : argument(s) missing" );
		lua_error( p_L );		
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    LuaClass_RenderStatesSet* lua_rss = Luna<LuaClass_RenderStatesSet>::check( p_L, 2 );

    if( m_passes.count( pass_id ) )
    {
        DrawSpace::Core::RenderStatesSet& rss = lua_rss->GetRenderStatesSet();

        m_passes[pass_id].m_fx.SetRenderStates( rss );
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::load_pass_viewportquad_shader : unknown pass id" );
        lua_error( p_L );	
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_updaterenderingqueues( lua_State* p_L )
{
    m_rendergraph.PushSignal_UpdatedRenderingQueues();
    return 0;
}