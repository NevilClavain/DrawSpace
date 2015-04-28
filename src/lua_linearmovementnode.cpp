/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "lua_linearmovementnode.h"
#include "luacontext.h"
#include "exceptions.h"

#include "lua_drawspace.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

const char LuaLinearMovementNode::className[] = "LinearMovementNode";
const Luna2<LuaLinearMovementNode>::RegType LuaLinearMovementNode::methods[] =
{
  { "LinkTo", &LuaLinearMovementNode::Lua_LinkTo },
  { "LoadScript", &LuaLinearMovementNode::Lua_LoadScript },
  { "SetInitpos", &LuaLinearMovementNode::Lua_SetInitpos },
  { "SetDirection", &LuaLinearMovementNode::Lua_SetDirection },
  { "SetInitialTheta", &LuaLinearMovementNode::Lua_SetInitialTheta },
  { "SetInitialPhi", &LuaLinearMovementNode::Lua_SetInitialPhi },
  { 0, 0 }
};

LuaLinearMovementNode::LuaLinearMovementNode( lua_State* p_L )
: m_linear_node( "linear_node" ),
m_existing_linear_node( NULL ),
m_initial_theta( 0.0 ),
m_initial_phi( 0.0 )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "LinearMovementNode ctor : bad number of args" );
		lua_error( p_L );		
	}
    const char* scene_name = luaL_checkstring( p_L, 1 );
    m_linear_node.SetSceneName( scene_name );

    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "LinearMovementNode:LinearMovementNode" );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<SceneNode<LinearMovement>**>( "existing_node", &m_existing_linear_node );

        (*m_scriptcalls_handler)( props );
    }
}

LuaLinearMovementNode::~LuaLinearMovementNode( void ) 
{
}

int LuaLinearMovementNode::Lua_LinkTo( lua_State* p_L )
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
    m_linear_node.GetSceneName( scene_name );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "LinearMovementNode:LinkTo" );
        props.AddPropValue<dsstring>( "scenegraph_name", scenegraph_name );        
        props.AddPropValue<dsstring>( "parent_name", parent_name );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<BaseSceneNode*>( "node", &m_linear_node );
        props.AddPropValue<Vector>( "init_pos", m_initpos );
        props.AddPropValue<Vector>( "dir", m_dir );
        props.AddPropValue<dsreal>( "init_theta", m_initial_theta );
        props.AddPropValue<dsreal>( "init_phi", m_initial_phi );

        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaLinearMovementNode::Lua_LoadScript( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "LoadScript : bad number of args" );
		lua_error( p_L );		
	}
    const char* filepath = luaL_checkstring( p_L, 1 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "LinearMovementNode:LoadScript" );
        props.AddPropValue<dsstring>( "filepath", filepath );
        if( m_existing_linear_node )
        {
            props.AddPropValue<BaseSceneNode*>( "node", m_existing_linear_node );
        }
        else
        {
            props.AddPropValue<BaseSceneNode*>( "node", &m_linear_node );
        }

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaLinearMovementNode::Lua_SetInitpos( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetInitpos : bad number of args" );
		lua_error( p_L );		
	}

    m_initpos[0] = luaL_checknumber( p_L, 1 );
    m_initpos[1] = luaL_checknumber( p_L, 2 );
    m_initpos[2] = luaL_checknumber( p_L, 3 );
    m_initpos[3] = 1.0;

    return 0;
}

int LuaLinearMovementNode::Lua_SetDirection( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetDirection : bad number of args" );
		lua_error( p_L );		
	}

    m_dir[0] = luaL_checknumber( p_L, 1 );
    m_dir[1] = luaL_checknumber( p_L, 2 );
    m_dir[2] = luaL_checknumber( p_L, 3 );
    m_dir[3] = 1.0;

    return 0;
}

int LuaLinearMovementNode::Lua_SetInitialTheta( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetInitialTheta : bad number of args" );
		lua_error( p_L );		
	}

    m_initial_phi = luaL_checknumber( p_L, 1 );
    return 0;
}

int LuaLinearMovementNode::Lua_SetInitialPhi( lua_State* p_L )
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
