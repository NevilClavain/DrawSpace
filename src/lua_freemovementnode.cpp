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

#include "lua_freemovementnode.h"
#include "luacontext.h"
#include "exceptions.h"

#include "lua_drawspace.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


const char LuaFreeMovementNode::className[] = "FreeMovementNode";
const Luna2<LuaFreeMovementNode>::RegType LuaFreeMovementNode::methods[] =
{
  { "LinkTo", &LuaFreeMovementNode::Lua_LinkTo },
  { "SetInitpos", &LuaFreeMovementNode::Lua_SetInitpos },
  { "RotateYaw", &LuaFreeMovementNode::Lua_RotateYaw },
  { "RotatePitch", &LuaFreeMovementNode::Lua_RotatePitch },
  { "RotateRoll", &LuaFreeMovementNode::Lua_RotateRoll },
  { "SetSpeed", &LuaFreeMovementNode::Lua_SetSpeed },
  { 0, 0 }
};

LuaFreeMovementNode::LuaFreeMovementNode( lua_State* p_L )
: m_free_node( "free_node" ),
m_existing_free_node( NULL )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "FreeMovementNode ctor : bad number of args" );
		lua_error( p_L );		
	}
    const char* scene_name = luaL_checkstring( p_L, 1 );
    m_free_node.SetSceneName( scene_name );

    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "FreeMovementNode:FreeMovementNode" );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<SceneNode<FreeMovement>**>( "existing_node", &m_existing_free_node );

        (*m_scriptcalls_handler)( props );
    }
}

LuaFreeMovementNode::~LuaFreeMovementNode( void ) 
{
}

int LuaFreeMovementNode::Lua_LinkTo( lua_State* p_L )
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
    m_free_node.GetSceneName( scene_name );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "FreeMovementNode:LinkTo" );
        props.AddPropValue<dsstring>( "scenegraph_name", scenegraph_name );        
        props.AddPropValue<dsstring>( "parent_name", parent_name );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<BaseSceneNode*>( "node", &m_free_node );
        props.AddPropValue<Vector>( "init_pos", m_initpos );

        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaFreeMovementNode::Lua_SetInitpos( lua_State* p_L )
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

int LuaFreeMovementNode::Lua_SetSpeed( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetSpeed : bad number of args" );
		lua_error( p_L );		
	}

    dsreal speed = luaL_checknumber( p_L, 1 );

    if( m_existing_free_node )
    {
        m_existing_free_node->GetContent()->SetSpeed( speed );
    }
    else
    {
        m_free_node.GetContent()->SetSpeed( speed );
    }

    return 0;
}

int LuaFreeMovementNode::Lua_RotateYaw( lua_State* p_L )
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

    if( m_existing_free_node )
    {
        m_existing_free_node->GetContent()->RotateYaw( speed, *ds->m_timer );
    }
    else
    {
        m_free_node.GetContent()->RotateYaw( speed, *ds->m_timer );
    }

    return 0;
}
int LuaFreeMovementNode::Lua_RotatePitch( lua_State* p_L )
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

    if( m_existing_free_node )
    {
        m_existing_free_node->GetContent()->RotatePitch( speed, *ds->m_timer );
    }
    else
    {
        m_free_node.GetContent()->RotatePitch( speed, *ds->m_timer );
    }

    return 0;
}
int LuaFreeMovementNode::Lua_RotateRoll( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "RotateRoll : bad number of args" );
		lua_error( p_L );		
	}

    dsreal speed = luaL_checknumber( p_L, 1 );
    LuaDrawSpace* ds = Luna2<LuaDrawSpace>::check( p_L, 2 );
    if( !ds )
    {
		lua_pushstring( p_L, "RotateRoll : Drawspace expected for arg 2" );
		lua_error( p_L );        
    }

    if( m_existing_free_node )
    {
        m_existing_free_node->GetContent()->RotateRoll( speed, *ds->m_timer );
    }
    else
    {
        m_free_node.GetContent()->RotateRoll( speed, *ds->m_timer );
    }

    return 0;
}
