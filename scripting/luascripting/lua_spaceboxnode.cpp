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

#include "lua_spaceboxnode.h"
#include "luacontext.h"
#include "lua_vector.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaSpacebox::className[] = "SpaceBox";
const Luna2<LuaSpacebox>::RegType LuaSpacebox::methods[] =
{  
  { "RegisterPassSlot", &LuaSpacebox::Lua_RegisterPassSlot },
  { "SetPassSlotFxName", &LuaSpacebox::Lua_SetPassSlotFxName },
  { "SetPassSlotRenderingOrder", &LuaSpacebox::Lua_SetPassSlotRenderingOrder },
  { "SetPassSlotTextureName", &LuaSpacebox::Lua_SetPassSlotTextureName },
  { "AddPassSlotShaderParam", &LuaSpacebox::Lua_AddPassSlotShaderParam },
  { "LinkTo", &LuaSpacebox::Lua_LinkTo },
  { 0 }
};


LuaSpacebox::LuaSpacebox( lua_State* p_L ) :
m_spacebox_node( "spacebox_node" )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "Spacebox ctor : bad number of args" );
		lua_error( p_L );		
	}
    const char* scene_name = luaL_checkstring( p_L, 1 );
    m_spacebox_node.SetSceneName( scene_name );

    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();    
}

LuaSpacebox::~LuaSpacebox( void ) 
{
}


int LuaSpacebox::Lua_LinkTo( lua_State* p_L )
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
    m_spacebox_node.GetSceneName( scene_name );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "SpaceboxNode:LinkTo" );
        props.AddPropValue<dsstring>( "scenegraph_name", scenegraph_name );        
        props.AddPropValue<dsstring>( "parent_name", parent_name );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<BaseSceneNode*>( "node", &m_spacebox_node );

        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaSpacebox::Lua_RegisterPassSlot( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "RegisterPassSlot : bad number of args" );
		lua_error( p_L );		
	}
    const char* pass_name = luaL_checkstring( p_L, 2 );

    PassDescriptor pass_descriptor;
    pass_descriptor.rendering_order = 200;
    m_descriptor.passes_slots[pass_name] = pass_descriptor;

    return 0;
}

int LuaSpacebox::Lua_SetPassSlotFxName( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetPassSlotFxName : bad number of args" );
		lua_error( p_L );		
	}
    const char* pass_name = luaL_checkstring( p_L, 1 );
    const char* fx_name = luaL_checkstring( p_L, 2 );

    if( 0 == m_descriptor.passes_slots.count( pass_name ) )
    {
        _DSEXCEPTION( "scripting FATAL : unknown pass" );
    }
    else
    {
        m_descriptor.passes_slots[pass_name].fx_name = fx_name;
    }
    return 0;
}
int LuaSpacebox::Lua_SetPassSlotRenderingOrder( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetPassSlotRenderingOrder : bad number of args" );
		lua_error( p_L );		
	}
    const char* pass_name = luaL_checkstring( p_L, 1 );
    long order = luaL_checkinteger( p_L, 2 );
    
    if( 0 == m_descriptor.passes_slots.count( pass_name ) )
    {
        _DSEXCEPTION( "scripting FATAL : unknown pass" );
    }
    else
    {
        m_descriptor.passes_slots[pass_name].rendering_order = order;
    }
    return 0;
}
int LuaSpacebox::Lua_SetPassSlotTextureName( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 4 )
	{
		lua_pushstring( p_L, "SetPassSlotTextureName : bad number of args" );
		lua_error( p_L );		
	}
    const char* pass_name = luaL_checkstring( p_L, 1 );
    const char* texture_name = luaL_checkstring( p_L, 2 );
    long face_id = luaL_checkinteger( p_L, 3 );
    long stage = luaL_checkinteger( p_L, 4 );
    
    if( 0 == m_descriptor.passes_slots.count( pass_name ) )
    {
        _DSEXCEPTION( "scripting FATAL : unknown pass" );
    }
    else
    {
        m_descriptor.passes_slots[pass_name].textures[face_id][stage] = texture_name;
    }
    return 0;
}

int LuaSpacebox::Lua_AddPassSlotShaderParam( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 4 )
	{
		lua_pushstring( p_L, "AddPassSlotShaderParam : bad number of args" );
		lua_error( p_L );		
	}

    const char* pass_name = luaL_checkstring( p_L, 1 );
    const char* id = luaL_checkstring( p_L, 2 );
    long shader_index = luaL_checkinteger( p_L, 3 );
    long shader_register = luaL_checkinteger( p_L, 4 );
    LuaVector* value = Luna2<LuaVector>::check( p_L, 5 );

    PassShaderParam psp;

    psp.id = id;
    psp.shader_index = shader_index;
    psp.shader_register = shader_register;
    psp.value = value->m_vector;

    if( 0 == m_descriptor.passes_slots.count( pass_name ) )
    {
        _DSEXCEPTION( "scripting FATAL : unknown pass" );
    }
    else
    {
        m_descriptor.passes_slots[pass_name].shader_params.push_back( psp );
    }
    return 0;
}