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

#include "lua_drawspace.h"
#include "luacontext.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaDrawSpace::className[] = "DrawSpace";
const Luna<LuaDrawSpace>::RegType LuaDrawSpace::Register[] =
{
  { "DisplayFramerate", &LuaDrawSpace::Lua_DisplayFramerate },
  { "DisplayCurrentCamera", &LuaDrawSpace::Lua_DisplayCurrentCamera },
  { "CreateSceneNodeGraph", &LuaDrawSpace::Lua_CreateSceneNodeGraph },
  { "CreateConstRegister", &LuaDrawSpace::Lua_CreateConstRegister },
  { "ModifyConstRegisterValue", &LuaDrawSpace::Lua_ModifyConstRegisterValue },
  { "CreateVarRegister", &LuaDrawSpace::Lua_CreateVarRegister },
  { "ResetVarRegister", &LuaDrawSpace::Lua_ResetVarRegister },
  { "SetVarRegisterState", &LuaDrawSpace::Lua_SetVarRegisterState },
  { "SetVarRegisterInitVal", &LuaDrawSpace::Lua_SetVarRegisterInitVal },
  { "SetVarRegisterSpeed", &LuaDrawSpace::Lua_SetVarRegisterSpeed },
  { "SetVarRegisterRange", &LuaDrawSpace::Lua_SetVarRegisterRange },
  { 0 }
};


LuaDrawSpace::LuaDrawSpace( lua_State* p_L )
{   
    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();
}

LuaDrawSpace::~LuaDrawSpace( void ) 
{
}

int LuaDrawSpace::Lua_DisplayFramerate( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "DisplayFramerate : bad number of args" );
		lua_error( p_L );		
	}

    bool display = ( (int)luaL_checkinteger( p_L, 2 ) != 0 ? true : false );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:DisplayFramerate" );
        props.AddPropValue<bool>( "state", display );

        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaDrawSpace::Lua_DisplayCurrentCamera( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "DisplayCurrentCamera : bad number of args" );
		lua_error( p_L );		
	}

    bool display = ( (int)luaL_checkinteger( p_L, 2 ) != 0 ? true : false );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:DisplayCurrentCamera" );
        props.AddPropValue<bool>( "state", display );

        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaDrawSpace::Lua_CreateSceneNodeGraph( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "CreateSceneNodeGraph : bad number of args" );
		lua_error( p_L );		
	}
    const char* name = luaL_checkstring( p_L, 2 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:CreateSceneNodeGraph" );
        props.AddPropValue<dsstring>( "name", name );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_CreateConstRegister( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "CreateConstRegister : bad number of args" );
		lua_error( p_L );		
	}

    const char* reg_name = luaL_checkstring( p_L, 2 );
    dsreal reg_val = luaL_checknumber( p_L, 3 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:CreateConstRegister" );
        props.AddPropValue<dsstring>( "reg_name", reg_name );
        props.AddPropValue<dsreal>( "reg_val", reg_val );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_ModifyConstRegisterValue( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "ModifyConstRegisterValue : bad number of args" );
		lua_error( p_L );		
	}

    const char* reg_name = luaL_checkstring( p_L, 2 );
    dsreal reg_val = luaL_checknumber( p_L, 3 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:ModifyConstRegisterValue" );
        props.AddPropValue<dsstring>( "reg_name", reg_name );
        props.AddPropValue<dsreal>( "reg_val", reg_val );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_CreateVarRegister( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 7 )
	{
		lua_pushstring( p_L, "CreateVarRegister : bad number of args" );
		lua_error( p_L );		
	}

    const char* reg_name = luaL_checkstring( p_L, 2 );
    const char* var_mode = luaL_checkstring( p_L, 3 );
    dsreal init_val = luaL_checknumber( p_L, 4 );
    dsreal speed = luaL_checknumber( p_L, 5 );
    dsreal range_inf = luaL_checknumber( p_L, 6 );
    dsreal range_sup = luaL_checknumber( p_L, 7 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:CreateVarRegister" );
        props.AddPropValue<dsstring>( "reg_name", reg_name );
        props.AddPropValue<dsstring>( "var_mode", var_mode );
        props.AddPropValue<dsreal>( "init_val", init_val );
        props.AddPropValue<dsreal>( "speed", speed );
        props.AddPropValue<dsreal>( "range_inf", range_inf );
        props.AddPropValue<dsreal>( "range_sup", range_sup );
        
        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_ResetVarRegister( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "ResetVarRegister : bad number of args" );
		lua_error( p_L );		
	}

    const char* reg_name = luaL_checkstring( p_L, 2 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:ResetVarRegister" );
        props.AddPropValue<dsstring>( "reg_name", reg_name );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_SetVarRegisterState( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetVarRegisterState : bad number of args" );
		lua_error( p_L );		
	}

    const char* reg_name = luaL_checkstring( p_L, 2 );
    bool state = (bool)luaL_checkinteger( p_L, 3 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:SetVarRegisterState" );
        props.AddPropValue<dsstring>( "reg_name", reg_name );
        props.AddPropValue<bool>( "state", state );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_SetVarRegisterInitVal( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetVarRegisterInitVal : bad number of args" );
		lua_error( p_L );		
	}

    const char* reg_name = luaL_checkstring( p_L, 2 );
    dsreal init_val = luaL_checknumber( p_L, 3 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:SetVarRegisterInitVal" );
        props.AddPropValue<dsstring>( "reg_name", reg_name );
        props.AddPropValue<dsreal>( "init_val", init_val );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_SetVarRegisterSpeed( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetVarRegisterSpeed : bad number of args" );
		lua_error( p_L );		
	}

    const char* reg_name = luaL_checkstring( p_L, 2 );
    dsreal speed = luaL_checknumber( p_L, 3 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:SetVarRegisterSpeed" );
        props.AddPropValue<dsstring>( "reg_name", reg_name );
        props.AddPropValue<dsreal>( "speed", speed );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_SetVarRegisterRange( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 4 )
	{
		lua_pushstring( p_L, "SetVarRegisterRange : bad number of args" );
		lua_error( p_L );		
	}

    const char* reg_name = luaL_checkstring( p_L, 2 );
    dsreal range_min = luaL_checknumber( p_L, 3 );
    dsreal range_max = luaL_checknumber( p_L, 4 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:SetVarRegisterRange" );
        props.AddPropValue<dsstring>( "reg_name", reg_name );
        props.AddPropValue<dsreal>( "range_min", range_min );
        props.AddPropValue<dsreal>( "range_max", range_max );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}
