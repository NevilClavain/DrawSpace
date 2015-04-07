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
#include "lua_real.h"
#include "luacontext.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaDrawSpace::className[] = "DrawSpace";
const Luna2<LuaDrawSpace>::RegType LuaDrawSpace::methods[] =
{
  { "DisplayFramerate", &LuaDrawSpace::Lua_DisplayFramerate },
  { "DisplayCurrentCamera", &LuaDrawSpace::Lua_DisplayCurrentCamera },
  { "CreateSceneNodeGraph", &LuaDrawSpace::Lua_CreateSceneNodeGraph },
  { "LoadKeyUpScript", &LuaDrawSpace::Lua_LoadKeyUpScript },
  { "LoadKeyDownScript", &LuaDrawSpace::Lua_LoadKeyDownScript },
  { "LoadMouseScript", &LuaDrawSpace::Lua_LoadMouseScript },
  { "AngleSpeedInc", &LuaDrawSpace::Lua_AngleSpeedInc },
  { "AngleSpeedDec", &LuaDrawSpace::Lua_AngleSpeedDec },
  { "TranslationSpeedInc", &LuaDrawSpace::Lua_TranslationSpeedInc },
  { "TranslationSpeedDec", &LuaDrawSpace::Lua_TranslationSpeedDec },
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
	if( argc != /*2*/ 1 )
	{
		lua_pushstring( p_L, "DisplayFramerate : bad number of args" );
		lua_error( p_L );		
	}

    bool display = ( (int)luaL_checkinteger( p_L, /*2*/ 1 ) != 0 ? true : false );

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
	if( argc != /*2*/ 1 )
	{
		lua_pushstring( p_L, "DisplayCurrentCamera : bad number of args" );
		lua_error( p_L );		
	}

    bool display = ( (int)luaL_checkinteger( p_L, /*2*/ 1 ) != 0 ? true : false );

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
	if( argc != /* 2 */ 1 )
	{
		lua_pushstring( p_L, "CreateSceneNodeGraph : bad number of args" );
		lua_error( p_L );		
	}
    const char* name = luaL_checkstring( p_L, /*2*/ 1 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:CreateSceneNodeGraph" );
        props.AddPropValue<dsstring>( "name", name );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_LoadKeyUpScript( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "LoadKeyUpScript : bad number of args" );
		lua_error( p_L );		
	}
    const char* filepath = luaL_checkstring( p_L, 1 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:LoadKeyUpScript" );
        props.AddPropValue<dsstring>( "filepath", filepath );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_LoadKeyDownScript( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "LoadKeyDownScript : bad number of args" );
		lua_error( p_L );		
	}
    const char* filepath = luaL_checkstring( p_L, 1 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:LoadKeyDownScript" );
        props.AddPropValue<dsstring>( "filepath", filepath );

        (*m_scriptcalls_handler)( props );
    }
    return 0;

}

int LuaDrawSpace::Lua_LoadMouseScript( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "LoadMouseScript : bad number of args" );
		lua_error( p_L );		
	}
    const char* filepath = luaL_checkstring( p_L, 1 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:LoadMouseScript" );
        props.AddPropValue<dsstring>( "filepath", filepath );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_AngleSpeedInc( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "AngleSpeedInc : bad number of args" );
		lua_error( p_L );		
	}
    LuaReal* real = Luna2<LuaReal>::check( p_L, 1 );
    if( !real )
    {
		lua_pushstring( p_L, "AngleSpeedInc : Real expected for arg 1" );
		lua_error( p_L );        
    }
    dsreal speed = luaL_checknumber( p_L, 2 );

    PropertyPool props;
    props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:AngleSpeedInc" );
    props.AddPropValue<dsreal>( "speed", speed );
    props.AddPropValue<dsreal*>( "realvar", &real->m_value );
    (*m_scriptcalls_handler)( props );

    return 0;
}

int LuaDrawSpace::Lua_AngleSpeedDec( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "AngleSpeedDec : bad number of args" );
		lua_error( p_L );		
	}
    LuaReal* real = Luna2<LuaReal>::check( p_L, 1 );
    if( !real )
    {
		lua_pushstring( p_L, "AngleSpeedDec : Real expected for arg 1" );
		lua_error( p_L );        
    }
    dsreal speed = luaL_checknumber( p_L, 2 );

    PropertyPool props;
    props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:AngleSpeedInc" );
    props.AddPropValue<dsreal>( "speed", speed );
    props.AddPropValue<dsreal*>( "realvar", &real->m_value );
    (*m_scriptcalls_handler)( props );

    return 0;
}

int LuaDrawSpace::Lua_TranslationSpeedInc( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "TranslationSpeedInc : bad number of args" );
		lua_error( p_L );		
	}
    LuaReal* real = Luna2<LuaReal>::check( p_L, 1 );
    if( !real )
    {
		lua_pushstring( p_L, "TranslationSpeedInc : Real expected for arg 1" );
		lua_error( p_L );        
    }
    dsreal speed = luaL_checknumber( p_L, 2 );

    PropertyPool props;
    props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:TranslationSpeedInc" );
    props.AddPropValue<dsreal>( "speed", speed );
    props.AddPropValue<dsreal*>( "realvar", &real->m_value );
    (*m_scriptcalls_handler)( props );

    return 0;
}

int LuaDrawSpace::Lua_TranslationSpeedDec( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "TranslationSpeedDec : bad number of args" );
		lua_error( p_L );		
	}
    LuaReal* real = Luna2<LuaReal>::check( p_L, 1 );
    if( !real )
    {
		lua_pushstring( p_L, "TranslationSpeedDec : Real expected for arg 1" );
		lua_error( p_L );        
    }
    dsreal speed = luaL_checknumber( p_L, 2 );

    PropertyPool props;
    props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:TranslationSpeedDec" );
    props.AddPropValue<dsreal>( "speed", speed );
    props.AddPropValue<dsreal*>( "realvar", &real->m_value );
    (*m_scriptcalls_handler)( props );

    return 0;
}
