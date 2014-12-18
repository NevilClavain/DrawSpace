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

#include "lua_drawspace.h"
#include "luacontext.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaDrawSpace::className[] = "DrawSpace";
const Luna<LuaDrawSpace>::RegType LuaDrawSpace::Register[] =
{
  { "DisplayFramerate", &LuaDrawSpace::Lua_DisplayFramerate },
  { "DisplayCurrentCamera", &LuaDrawSpace::Lua_DisplayCurrentCamera },
  { "CreateConstRegister", &LuaDrawSpace::Lua_CreateConstRegister },
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