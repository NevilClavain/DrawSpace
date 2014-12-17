/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "lua_spaceboxbuilder.h"
#include "luacontext.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaSpaceboxBuilder::className[] = "SpaceBoxBuilder";
const Luna<LuaSpaceboxBuilder>::RegType LuaSpaceboxBuilder::Register[] =
{
  { "SetSceneName", &LuaSpaceboxBuilder::Lua_SetSceneName },
  { "RegisterPassSlot", &LuaSpaceboxBuilder::Lua_RegisterPassSlot },
  { "SetPassSlotFxName", &LuaSpaceboxBuilder::Lua_SetPassSlotFxName },
  { "SetPassSlotRenderingOrder", &LuaSpaceboxBuilder::Lua_SetPassSlotRenderingOrder },
  { "SetPassSlotTextureName", &LuaSpaceboxBuilder::Lua_SetPassSlotTextureName },
  { "BuildIt", &LuaSpaceboxBuilder::Lua_BuildIt },
  { "ClearMatrixStack", &LuaSpaceboxBuilder::Lua_ClearMatrixStack },
  { "AddMatrix", &LuaSpaceboxBuilder::Lua_AddMatrix },
  { 0 }
};


LuaSpaceboxBuilder::LuaSpaceboxBuilder( lua_State* p_L )
{   
    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();
}

LuaSpaceboxBuilder::~LuaSpaceboxBuilder( void ) 
{
}

int LuaSpaceboxBuilder::Lua_SetSceneName( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetSceneName : bad number of args" );
		lua_error( p_L );		
	}
    const char* scene_name = luaL_checkstring( p_L, 2 );
    m_descriptor.scene_name = scene_name;

    return 0;
}

int LuaSpaceboxBuilder::Lua_RegisterPassSlot( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "RegisterPassSlot : bad number of args" );
		lua_error( p_L );		
	}
    const char* pass_name = luaL_checkstring( p_L, 2 );

    Spacebox::PassDescriptor pass_descriptor;
    pass_descriptor.rendering_order = 200;
    m_descriptor.passes_slots[pass_name] = pass_descriptor;

    return 0;
}

int LuaSpaceboxBuilder::Lua_SetPassSlotFxName( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetPassSlotFxName : bad number of args" );
		lua_error( p_L );		
	}
    const char* pass_name = luaL_checkstring( p_L, 2 );
    const char* fx_name = luaL_checkstring( p_L, 3 );

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
int LuaSpaceboxBuilder::Lua_SetPassSlotRenderingOrder( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetPassSlotRenderingOrder : bad number of args" );
		lua_error( p_L );		
	}
    const char* pass_name = luaL_checkstring( p_L, 2 );
    long order = luaL_checkinteger( p_L, 3 );
    
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
int LuaSpaceboxBuilder::Lua_SetPassSlotTextureName( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 5 )
	{
		lua_pushstring( p_L, "SetPassSlotTextureName : bad number of args" );
		lua_error( p_L );		
	}
    const char* pass_name = luaL_checkstring( p_L, 2 );
    const char* texture_name = luaL_checkstring( p_L, 3 );
    long face_id = luaL_checkinteger( p_L, 4 );
    long stage = luaL_checkinteger( p_L, 5 );
    
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

int LuaSpaceboxBuilder::Lua_BuildIt( lua_State* p_L )
{
    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "SpaceBoxBuilder:BuildIt" );
        props.AddPropValue<Spacebox::Descriptor>( "descr", m_descriptor );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaSpaceboxBuilder::Lua_ClearMatrixStack( lua_State* p_L )
{
    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "SpaceBoxBuilder:ClearMatrixStack" );
        props.AddPropValue<dsstring>( "name", m_descriptor.scene_name );
        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaSpaceboxBuilder::Lua_AddMatrix( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 8 )
	{
		lua_pushstring( p_L, "AddMatrix : bad number of args" );
		lua_error( p_L );		
	}

    dsreal x, y, z, angle;
    const char* xreg;
    const char* yreg;
    const char* zreg;
    const char* anglereg;

    dsstring mat_type = luaL_checkstring( p_L, 2 );

    x = luaL_checknumber( p_L, 3 );
    xreg = luaL_checkstring( p_L, 4 );

    y = luaL_checknumber( p_L, 5 );
    yreg = luaL_checkstring( p_L, 6 );

    z = luaL_checknumber( p_L, 7 );
    zreg = luaL_checkstring( p_L, 8 );

    if( "rotation" == mat_type )
    {
	    if( argc < 10 )
	    {
		    lua_pushstring( p_L, "AddMatrix : bad number of args" );
		    lua_error( p_L );		
	    }

        angle = luaL_checknumber( p_L, 9 );
        anglereg = luaL_checkstring( p_L, 10 );
    }
    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "SpaceBoxBuilder:AddMatrix" );

        props.AddPropValue<dsstring>( "name", m_descriptor.scene_name );

        props.AddPropValue<dsstring>( "mat_type", mat_type );

        props.AddPropValue<dsreal>( "x", x );
        props.AddPropValue<dsstring>( "xreg", xreg );

        props.AddPropValue<dsreal>( "y", y );
        props.AddPropValue<dsstring>( "yreg", yreg );

        props.AddPropValue<dsreal>( "z", z );
        props.AddPropValue<dsstring>( "zreg", zreg );

        if( "rotation" == mat_type )
        {
            props.AddPropValue<dsreal>( "angle", angle );
            props.AddPropValue<dsstring>( "areg", anglereg );
        }

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}
