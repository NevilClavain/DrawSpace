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
  { "SetCenterpos", &LuaCircularMovementNode::Lua_SetCenterpos },
  { "SetDeltaCenterpos", &LuaCircularMovementNode::Lua_SetDeltaCenterpos },
  { "SetAxis", &LuaCircularMovementNode::Lua_SetAxis },
  { "SetInitialTheta", &LuaCircularMovementNode::Lua_SetInitialTheta },
  { "SetInitialPhi", &LuaCircularMovementNode::Lua_SetInitialPhi },
  { "SetInitialAngle", &LuaCircularMovementNode::Lua_SetInitialAngle },
  { "SetTheta", &LuaCircularMovementNode::Lua_SetTheta },
  { "SetPhi", &LuaCircularMovementNode::Lua_SetPhi },
  { "SetAngularSpeed", &LuaCircularMovementNode::Lua_SetAngularSpeed },
  { 0, 0 }
};

LuaCircularMovementNode::LuaCircularMovementNode( lua_State* p_L )
: m_circular_node( "circular_node" ),
m_existing_circular_node( NULL ),
m_initial_theta( 0.0 ),
m_initial_phi( 0.0 ),
m_initial_angle( 0.0 )
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
        props.AddPropValue<Vector>( "center_pos", m_centerpos );
        props.AddPropValue<Vector>( "delta_center_pos", m_deltacenterpos );
        props.AddPropValue<Vector>( "axis", m_axis );
        props.AddPropValue<dsreal>( "init_theta", m_initial_theta );
        props.AddPropValue<dsreal>( "init_phi", m_initial_phi );
        props.AddPropValue<dsreal>( "init_angle", m_initial_angle );
        
        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaCircularMovementNode::Lua_SetCenterpos( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetCenterpos : bad number of args" );
		lua_error( p_L );		
	}

    m_centerpos[0] = luaL_checknumber( p_L, 1 );
    m_centerpos[1] = luaL_checknumber( p_L, 2 );
    m_centerpos[2] = luaL_checknumber( p_L, 3 );
    m_centerpos[3] = 1.0;

    return 0;
}

int LuaCircularMovementNode::Lua_SetDeltaCenterpos( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetDeltaCenterpos : bad number of args" );
		lua_error( p_L );		
	}

    m_deltacenterpos[0] = luaL_checknumber( p_L, 1 );
    m_deltacenterpos[1] = luaL_checknumber( p_L, 2 );
    m_deltacenterpos[2] = luaL_checknumber( p_L, 3 );
    m_deltacenterpos[3] = 1.0;

    return 0;
}

int LuaCircularMovementNode::Lua_SetAxis( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetAxis : bad number of args" );
		lua_error( p_L );		
	}

    m_axis[0] = luaL_checknumber( p_L, 1 );
    m_axis[1] = luaL_checknumber( p_L, 2 );
    m_axis[2] = luaL_checknumber( p_L, 3 );
    m_axis[3] = 1.0;

    return 0;
}

int LuaCircularMovementNode::Lua_SetInitialTheta( lua_State* p_L )
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

int LuaCircularMovementNode::Lua_SetInitialPhi( lua_State* p_L )
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

int LuaCircularMovementNode::Lua_SetInitialAngle( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetInitialAngle : bad number of args" );
		lua_error( p_L );		
	}

    m_initial_angle = luaL_checknumber( p_L, 1 );

    return 0;
}

int LuaCircularMovementNode::Lua_SetTheta( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetTheta : bad number of args" );
		lua_error( p_L );		
	}

    dsreal theta = luaL_checknumber( p_L, 1 );

    if( m_existing_circular_node )
    {
        m_existing_circular_node->GetContent()->SetTheta( theta );
    }
    else
    {
        m_circular_node.GetContent()->SetTheta( theta );
    }

    return 0;
}

int LuaCircularMovementNode::Lua_SetPhi( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetPhi : bad number of args" );
		lua_error( p_L );		
	}

    dsreal phi = luaL_checknumber( p_L, 1 );

    if( m_existing_circular_node )
    {
        m_existing_circular_node->GetContent()->SetPhi( phi );
    }
    else
    {
        m_circular_node.GetContent()->SetPhi( phi );
    }

    return 0;
}

int LuaCircularMovementNode::Lua_SetAngularSpeed( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetAngularSpeed : bad number of args" );
		lua_error( p_L );		
	}

    dsreal speed = luaL_checknumber( p_L, 1 );

    if( m_existing_circular_node )
    {
        m_existing_circular_node->GetContent()->SetAngularSpeed( speed );
    }
    else
    {
        m_circular_node.GetContent()->SetAngularSpeed( speed );
    }

    return 0;
}
