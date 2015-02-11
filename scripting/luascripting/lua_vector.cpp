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

#include "lua_vector.h"

const char LuaVector::className[] = "Vector";
const Luna2<LuaVector>::RegType LuaVector::methods[] =
{
    { "Set", &LuaVector::Lua_Set },
    { "Get", &LuaVector::Lua_Get },
    { "Length", &LuaVector::Lua_Lenght },
    { "LengthPow2", &LuaVector::Lua_LenghtPow2 },
    { "Normalize", &LuaVector::Lua_Normalize },
    { "Scale", &LuaVector::Lua_Scale },
    { 0, 0 }
};

LuaVector::LuaVector( lua_State* p_L )
{
    int argc = lua_gettop( p_L );
    if( argc > 4 ) argc = 4;

    for( int i = 0; i < argc; i++ )
    {
        dsreal val = luaL_checknumber( p_L, i + 1 );
        m_vector[i] = val;
    }
}

LuaVector::~LuaVector( void )
{

}

int LuaVector::Lua_Set( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "Vector Set : bad number of args" );
		lua_error( p_L );		
    }

    int index = luaL_checkinteger( p_L, 1 );
    if( index > 3 ) index = 3;
    dsreal val = luaL_checknumber( p_L, 2 );

    m_vector[index] = val;
    return 0;
}

int LuaVector::Lua_Get( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "Vector Get : bad number of args" );
		lua_error( p_L );		
    }

    int index = luaL_checkinteger( p_L, 1 );
    if( index > 3 ) index = 3;

    lua_pushnumber( p_L, m_vector[index] );
    return 1;
}

int LuaVector::Lua_Lenght( lua_State* p_L )
{
    dsreal length = m_vector.Length();

    lua_pushnumber( p_L, length );
    return 1;
}

int LuaVector::Lua_LenghtPow2( lua_State* p_L )
{
    dsreal lengthp2 = m_vector.LengthPow2();

    lua_pushnumber( p_L, lengthp2 );
    return 1;
}

int LuaVector::Lua_Normalize( lua_State* p_L )
{
    m_vector.Normalize();
    return 0;
}

int LuaVector::Lua_Scale( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "Vector Scale : bad number of args" );
		lua_error( p_L );		
    }
    dsreal scale = luaL_checknumber( p_L, 1 );

    m_vector.Scale( scale );

    return 0;
}
