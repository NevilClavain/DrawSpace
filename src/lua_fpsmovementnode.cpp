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

#include "lua_fpsmovementnode.h"
#include "luacontext.h"
#include "exceptions.h"

#include "lua_drawspace.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

const char LuaFpsMovementNode::className[] = "FpsMovementNode";
const Luna2<LuaFpsMovementNode>::RegType LuaFpsMovementNode::methods[] =
{
  { "LinkTo", &LuaFpsMovementNode::Lua_LinkTo },  
  { "SetInitpos", &LuaFpsMovementNode::Lua_SetInitpos },
  { "SetInitialTheta", &LuaFpsMovementNode::Lua_SetInitialTheta },
  { "SetInitialPhi", &LuaFpsMovementNode::Lua_SetInitialPhi },
  { "SetYMvt", &LuaFpsMovementNode::Lua_SetYMvt },
  { "RotateYaw", &LuaFpsMovementNode::Lua_RotateYaw },
  { "RotatePitch", &LuaFpsMovementNode::Lua_RotatePitch },
  { "SetSpeed", &LuaFpsMovementNode::Lua_SetSpeed },
  { 0, 0 }
};

LuaFpsMovementNode::LuaFpsMovementNode( lua_State* p_L )
: m_fps_node( "fps_node" ),
m_existing_fps_node( NULL ),
m_ymvt( false ),
m_initial_theta( 0.0 ),
m_initial_phi( 0.0 )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "FpsMovementNode ctor : bad number of args" );
		lua_error( p_L );		
	}
    const char* scene_name = luaL_checkstring( p_L, 1 );
    m_fps_node.SetSceneName( scene_name );

    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "FpsMovementNode:FpsMovementNode" );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<SceneNode<FPSMovement>**>( "existing_node", &m_existing_fps_node );

        (*m_scriptcalls_handler)( props );
    }
}

LuaFpsMovementNode::~LuaFpsMovementNode( void ) 
{
}

int LuaFpsMovementNode::Lua_LinkTo( lua_State* p_L )
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
    m_fps_node.GetSceneName( scene_name );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "FpsMovementNode:LinkTo" );
        props.AddPropValue<dsstring>( "scenegraph_name", scenegraph_name );        
        props.AddPropValue<dsstring>( "parent_name", parent_name );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<BaseSceneNode*>( "node", &m_fps_node );
        props.AddPropValue<Vector>( "init_pos", m_initpos );
        props.AddPropValue<dsreal>( "init_theta", m_initial_theta );
        props.AddPropValue<dsreal>( "init_phi", m_initial_phi );
        props.AddPropValue<bool>( "y_mvt", m_ymvt );

        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaFpsMovementNode::Lua_SetInitpos( lua_State* p_L )
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

int LuaFpsMovementNode::Lua_SetInitialTheta( lua_State* p_L )
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

int LuaFpsMovementNode::Lua_SetInitialPhi( lua_State* p_L )
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

int LuaFpsMovementNode::Lua_SetYMvt( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetYMvt : bad number of args" );
		lua_error( p_L );		
	}

    m_ymvt = luaL_checkinteger( p_L, 1 );
    return 0;
}

int LuaFpsMovementNode::Lua_RotateYaw( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "RotateYaw : bad number of args" );
		lua_error( p_L );		
	}

    dsreal speed = luaL_checknumber( p_L, 1 );
    LuaDrawSpace* ds = Luna2<LuaDrawSpace>::check( p_L, 2 );
    if( !ds )
    {
		lua_pushstring( p_L, "RotateYaw : Drawspace expected for arg 2" );
		lua_error( p_L );        
    }

    if( m_existing_fps_node )
    {
        m_existing_fps_node->GetContent()->RotateYaw( speed, *ds->m_timer );
    }
    else
    {
        m_fps_node.GetContent()->RotateYaw( speed, *ds->m_timer );
    }

    return 0;
}

int LuaFpsMovementNode::Lua_RotatePitch( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "RotatePitch : bad number of args" );
		lua_error( p_L );		
	}

    dsreal speed = luaL_checknumber( p_L, 1 );
    LuaDrawSpace* ds = Luna2<LuaDrawSpace>::check( p_L, 2 );
    if( !ds )
    {
		lua_pushstring( p_L, "RotatePitch : Drawspace expected for arg 2" );
		lua_error( p_L );        
    }

    if( m_existing_fps_node )
    {
        m_existing_fps_node->GetContent()->RotatePitch( speed, *ds->m_timer );
    }
    else
    {
        m_fps_node.GetContent()->RotatePitch( speed, *ds->m_timer );
    }

    return 0;
}

int LuaFpsMovementNode::Lua_SetSpeed( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetSpeed : bad number of args" );
		lua_error( p_L );		
	}

    dsreal speed = luaL_checknumber( p_L, 1 );

    if( m_existing_fps_node )
    {
        m_existing_fps_node->GetContent()->SetSpeed( speed );
    }
    else
    {
        m_fps_node.GetContent()->SetSpeed( speed );
    }

    return 0;
}