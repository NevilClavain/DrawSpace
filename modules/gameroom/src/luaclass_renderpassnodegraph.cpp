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
#include <drawspace_commons.h>
#include "mainservice.h"

const char LuaClass_RenderPassNodeGraph::className[] = "RenderPassNodeGraph";
const Luna<LuaClass_RenderPassNodeGraph>::RegType LuaClass_RenderPassNodeGraph::methods[] =
{
    { "create_rootpass", &LuaClass_RenderPassNodeGraph::LUA_createrootpass },
    { "set_pass_targetclearcolor", &LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearcolor },
    { "set_pass_targetclearstate", &LuaClass_RenderPassNodeGraph::LUA_setpasstargetclearstate },
    { "set_pass_depthclearstate", &LuaClass_RenderPassNodeGraph::LUA_setpassdepthclearstate },
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
    m_passes[pass_id] = m_rendergraph.CreateRoot( pass_id );

    // reglages par defaut
    m_passes[pass_id].GetRenderingQueue()->EnableDepthClearing( false );
    m_passes[pass_id].GetRenderingQueue()->EnableTargetClearing( true );
    m_passes[pass_id].GetRenderingQueue()->SetTargetClearingColor( 26, 27, 180, 255 );

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
        m_passes[pass_id].GetRenderingQueue()->SetTargetClearingColor( r, g, b, 255 );
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::set_pass_targetclearcolor : unknown pass id" );
    }

    //MainService::GetInstance()->RequestPassTargetClearColor( pass_id, r, g, b );
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
        m_passes[pass_id].GetRenderingQueue()->EnableTargetClearing( state );
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::set_pass_targetclearstate : unknown pass id" );
    }

    //MainService::GetInstance()->RequestPassTargetClearState( pass_id, state );
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
        m_passes[pass_id].GetRenderingQueue()->EnableDepthClearing( state );
    }
    else
    {
        lua_pushstring( p_L, "RenderPassNodeGraph::set_pass_depthclearstate : unknown pass id" );
    }

    //MainService::GetInstance()->RequestPassDepthClearState( pass_id, state );
    return 0;
}
