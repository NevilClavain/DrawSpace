/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#include "lua_longlatmovementnode.h"
#include "luacontext.h"
#include "exceptions.h"

#include "lua_drawspace.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

const char LuaLongLatMovementNode::className[] = "LongLatMovementNode";
const Luna2<LuaLongLatMovementNode>::RegType LuaLongLatMovementNode::methods[] =
{
  { "LinkTo", &LuaLongLatMovementNode::Lua_LinkTo },
  { "SetInitialTheta", &LuaLongLatMovementNode::Lua_SetInitialTheta },
  { "SetInitialPhi", &LuaLongLatMovementNode::Lua_SetInitialPhi },
  { "SetInitialAlt", &LuaLongLatMovementNode::Lua_SetInitialAlt },
  { "SetInitialLongitud", &LuaLongLatMovementNode::Lua_SetInitialLongitud },
  { "SetInitialLatitud", &LuaLongLatMovementNode::Lua_SetInitialLatitud },
  { "SetTheta", &LuaLongLatMovementNode::Lua_SetTheta },
  { "SetPhi", &LuaLongLatMovementNode::Lua_SetPhi },
  { "SetAlt", &LuaLongLatMovementNode::Lua_SetAlt },
  { "SetLongitud", &LuaLongLatMovementNode::Lua_SetLongitud },
  { "SetLatitud", &LuaLongLatMovementNode::Lua_SetLatitud },
  { 0, 0 }
};

LuaLongLatMovementNode::LuaLongLatMovementNode( lua_State* p_L )
: m_longlat_node( "longlat_node" ),
m_existing_longlat_node( NULL ),
m_initial_theta( 0.0 ),
m_initial_phi( 0.0 ),
m_initial_alt( 0.0 ),
m_initial_long( 0.0 ),
m_initial_lat( 0.0 )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "LongLatMovementNode ctor : bad number of args" );
		lua_error( p_L );		
	}
    const char* scene_name = luaL_checkstring( p_L, 1 );
    m_longlat_node.SetSceneName( scene_name );

    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "LongLatMovementNode:LongLatMovementNode" );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<SceneNode<LongLatMovement>**>( "existing_node", &m_existing_longlat_node );

        (*m_scriptcalls_handler)( props );
    }
}

LuaLongLatMovementNode::~LuaLongLatMovementNode( void ) 
{
}

int LuaLongLatMovementNode::Lua_LinkTo( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "LinkTo : bad number of args" );
		lua_error( p_L );		
	}
    const char* scenegraph_name = luaL_checkstring( p_L, 1 );
    const char* parent_name = luaL_checkstring( p_L, 2 );

    dsstring scene_name;
    m_longlat_node.GetSceneName( scene_name );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "LongLatMovementNode:LinkTo" );
        props.AddPropValue<dsstring>( "scenegraph_name", scenegraph_name );        
        props.AddPropValue<dsstring>( "parent_name", parent_name );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<BaseSceneNode*>( "node", &m_longlat_node );
        props.AddPropValue<dsreal>( "init_theta", m_initial_theta );
        props.AddPropValue<dsreal>( "init_phi", m_initial_phi );
        props.AddPropValue<dsreal>( "init_alt", m_initial_alt );
        props.AddPropValue<dsreal>( "init_long", m_initial_long );
        props.AddPropValue<dsreal>( "init_lat", m_initial_lat );
        
        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaLongLatMovementNode::Lua_SetInitialTheta( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetInitialTheta : bad number of args" );
		lua_error( p_L );		
	}

    m_initial_theta = luaL_checknumber( p_L, 1 );

    return 0;
}

int LuaLongLatMovementNode::Lua_SetInitialPhi( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetInitialPhi : bad number of args" );
		lua_error( p_L );		
	}

    m_initial_phi = luaL_checknumber( p_L, 1 );

    return 0;
}

int LuaLongLatMovementNode::Lua_SetInitialAlt( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetInitialAlt : bad number of args" );
		lua_error( p_L );		
	}

    m_initial_alt = luaL_checknumber( p_L, 1 );

    return 0;
}

int LuaLongLatMovementNode::Lua_SetInitialLongitud( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetInitialLongitud : bad number of args" );
		lua_error( p_L );		
	}

    m_initial_long = luaL_checknumber( p_L, 1 );

    return 0;
}

int LuaLongLatMovementNode::Lua_SetInitialLatitud( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetInitialLatitud : bad number of args" );
		lua_error( p_L );		
	}

    m_initial_lat = luaL_checknumber( p_L, 1 );

    return 0;
}

int LuaLongLatMovementNode::Lua_SetTheta( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetTheta : bad number of args" );
		lua_error( p_L );		
	}

    dsreal theta = luaL_checknumber( p_L, 1 );

    if( m_existing_longlat_node )
    {
        m_existing_longlat_node->GetContent()->SetTheta( theta );
    }
    else
    {
        m_longlat_node.GetContent()->SetTheta( theta );
    }

    return 0;
}

int LuaLongLatMovementNode::Lua_SetPhi( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetPhi : bad number of args" );
		lua_error( p_L );		
	}

    dsreal phi = luaL_checknumber( p_L, 1 );

    if( m_existing_longlat_node )
    {
        m_existing_longlat_node->GetContent()->SetPhi( phi );
    }
    else
    {
        m_longlat_node.GetContent()->SetPhi( phi );
    }

    return 0;
}

int LuaLongLatMovementNode::Lua_SetAlt( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetAlt : bad number of args" );
		lua_error( p_L );		
	}

    dsreal alt = luaL_checknumber( p_L, 1 );

    if( m_existing_longlat_node )
    {
        m_existing_longlat_node->GetContent()->SetAlt( alt );
    }
    else
    {
        m_longlat_node.GetContent()->SetAlt( alt );
    }

    return 0;
}

int LuaLongLatMovementNode::Lua_SetLongitud( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetLongitud : bad number of args" );
		lua_error( p_L );		
	}

    dsreal longit = luaL_checknumber( p_L, 1 );

    if( m_existing_longlat_node )
    {
        m_existing_longlat_node->GetContent()->SetLongitud( longit );
    }
    else
    {
        m_longlat_node.GetContent()->SetLongitud( longit );
    }
    return 0;
}

int LuaLongLatMovementNode::Lua_SetLatitud( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetLatitud : bad number of args" );
		lua_error( p_L );		
	}

    dsreal latit = luaL_checknumber( p_L, 1 );

    if( m_existing_longlat_node )
    {
        m_existing_longlat_node->GetContent()->SetLatitud( latit );
    }
    else
    {
        m_longlat_node.GetContent()->SetLatitud( latit );
    }

    return 0;
}
