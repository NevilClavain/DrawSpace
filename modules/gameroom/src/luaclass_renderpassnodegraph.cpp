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

const char LuaClass_RenderPassNodeGraph::className[] = "RenderPassNodeGraph";
const Luna<LuaClass_RenderPassNodeGraph>::RegType LuaClass_RenderPassNodeGraph::methods[] =
{
    { "create_rootpass", &LuaClass_RenderPassNodeGraph::LUA_createrootpass },
    { "set_pass_targetclearcolor", &LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearcolor },
    { "set_pass_targetclearstate", &LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearstate },
    { "set_pass_depthclearstate", &LuaClass_RenderPassNodeGraph::LUA_setpassdepthclearstate },
    { "create_pass_viewportquad", &LuaClass_RenderPassNodeGraph::LUA_createpassviewportquad },
    { "load_pass_viewportquad_shader", &LuaClass_RenderPassNodeGraph::LUA_loadpassviewportquadshader },
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

        m_passes[pass_id].m_fx.AddShader( &m_passes[pass_id].m_v_shader );
        m_passes[pass_id].m_fx.AddShader( &m_passes[pass_id].m_p_shader );
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::create_pass_viewportquad : unknown pass id" );
        lua_error( p_L );	
    }

    return 0;
}

int LuaClass_RenderPassNodeGraph::LUA_loadpassviewportquadshader( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 4 )
	{
		lua_pushstring( p_L, "RenderPassNodeGraph::load_pass_viewportquad_shader : argument(s) missing" );
		lua_error( p_L );		
	}

    dsstring pass_id = luaL_checkstring( p_L, 1 );
    int shader_index = luaL_checkint( p_L, 2 );
    dsstring shader_path = luaL_checkstring( p_L, 3 );
    bool is_compiled = luaL_checkstring( p_L, 4 );

    if( m_passes.count( pass_id ) )
    {
        bool status;
        if( 0 == shader_index )
        {
            status = m_passes[pass_id].m_v_shader.LoadFromFile( shader_path, is_compiled );
        }
        else if( 1 == shader_index )
        {
            status = m_passes[pass_id].m_p_shader.LoadFromFile( shader_path, is_compiled );
        }
        if( !status )
        {
            lua_pushstring( p_L, "RenderPassNodeGraph::load_pass_viewportquad_shader : shader loading operation failed" );
            lua_error( p_L );	        
        }
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::load_pass_viewportquad_shader : unknown pass id" );
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