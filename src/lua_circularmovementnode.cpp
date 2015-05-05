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

#include "lua_circularmovementnode.h"
#include "luacontext.h"
#include "exceptions.h"

#include "lua_drawspace.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

const char LuaCircularMovementNode::className[] = "CircularMovementNode";
const Luna2<LuaCircularMovementNode>::RegType LuaCircularMovementNode::methods[] =
{
  { "LinkTo", &LuaCircularMovementNode::Lua_LinkTo },
  { "LoadScript", &LuaCircularMovementNode::Lua_LoadScript },
  { "SetCenterPos", &LuaCircularMovementNode::Lua_SetCenterpos },
  { "SetDeltaCenterPos", &LuaCircularMovementNode::Lua_SetDeltaCenterpos },
  { "SetAxis", &LuaCircularMovementNode::Lua_SetAxis },
  { "SetInitialTheta", &LuaCircularMovementNode::Lua_SetInitialTheta },
  { "SetInitialPhi", &LuaCircularMovementNode::Lua_SetInitialPhi },
  { "SetInitialAngle", &LuaCircularMovementNode::Lua_SetInitialAngle },
  { 0, 0 }
};

LuaCircularMovementNode::LuaCircularMovementNode( lua_State* p_L )
: m_circular_node( "circular_node" ),
m_existing_circular_node( NULL )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "CircularMovementNode ctor : bad number of args" );
		lua_error( p_L );		
	}
    const char* scene_name = luaL_checkstring( p_L, 1 );
    m_circular_node.SetSceneName( scene_name );

    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "CircularMovementNode:CircularMovementNode" );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<SceneNode<CircularMovement>**>( "existing_node", &m_existing_circular_node );

        (*m_scriptcalls_handler)( props );
    }
}

LuaCircularMovementNode::~LuaCircularMovementNode( void ) 
{
}

int LuaCircularMovementNode::Lua_LinkTo( lua_State* p_L )
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
    m_circular_node.GetSceneName( scene_name );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "CircularMovementNode:LinkTo" );
        props.AddPropValue<dsstring>( "scenegraph_name", scenegraph_name );        
        props.AddPropValue<dsstring>( "parent_name", parent_name );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<BaseSceneNode*>( "node", &m_circular_node );
        /*props.AddPropValue<Vector>( "init_pos", m_initpos );
        props.AddPropValue<Vector>( "dir", m_dir );
        props.AddPropValue<dsreal>( "init_theta", m_initial_theta );
        props.AddPropValue<dsreal>( "init_phi", m_initial_phi );
        */
        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaCircularMovementNode::Lua_LoadScript( lua_State* p_L )
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

        props.AddPropValue<dsstring>( "script_call_id", "CircularMovementNode:LoadScript" );
        props.AddPropValue<dsstring>( "filepath", filepath );
        if( m_existing_circular_node )
        {
            props.AddPropValue<BaseSceneNode*>( "node", m_existing_circular_node );
        }
        else
        {
            props.AddPropValue<BaseSceneNode*>( "node", &m_circular_node );
        }

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaCircularMovementNode::Lua_SetCenterpos( lua_State* p_L )
{
    return 0;
}

int LuaCircularMovementNode::Lua_SetDeltaCenterpos( lua_State* p_L )
{
    return 0;
}

int LuaCircularMovementNode::Lua_SetAxis( lua_State* p_L )
{
    return 0;
}

int LuaCircularMovementNode::Lua_SetInitialTheta( lua_State* p_L )
{
    return 0;
}

int LuaCircularMovementNode::Lua_SetInitialPhi( lua_State* p_L )
{
    return 0;
}

int LuaCircularMovementNode::Lua_SetInitialAngle( lua_State* p_L )
{
    return 0;
}
