/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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
/* -*-LIC_END-*- */

#include "luacontext.h"
#include "luaclass_matrix.h"
#include "maths.h"

using namespace DrawSpace::Utils;

const char LuaClass_Matrix::className[] = "Matrix";
const Luna<LuaClass_Matrix>::RegType LuaClass_Matrix::methods[] =
{
    { "zero", &LuaClass_Matrix::LUA_zero },
    { "identity", &LuaClass_Matrix::LUA_identity },
    { "translation", &LuaClass_Matrix::LUA_translation },
    { "transpose", &LuaClass_Matrix::LUA_transpose },
    { "perspective", &LuaClass_Matrix::LUA_perspective },
    { "scale", &LuaClass_Matrix::LUA_scale },
    { "clear_translation", &LuaClass_Matrix::LUA_cleartranslation },
    { "rotation", &LuaClass_Matrix::LUA_rotation },
    { "inverse", &LuaClass_Matrix::LUA_inverse },
    { "set_value", &LuaClass_Matrix::LUA_setvalue },
    { "get_value", &LuaClass_Matrix::LUA_getvalue },
	{ 0, 0 }
};


LuaClass_Matrix::LuaClass_Matrix( lua_State* p_L )
{
}

LuaClass_Matrix::~LuaClass_Matrix( void )
{
}

int LuaClass_Matrix::LUA_zero( lua_State* p_L )
{
    m_matrix.Zero();
    return 0;
}

int LuaClass_Matrix::LUA_identity( lua_State* p_L )
{
    m_matrix.Identity();
    return 0;
}

int LuaClass_Matrix::LUA_translation( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{		
        LUA_ERROR( "Matrix::translation : argument(s) missing" );
	}

    dsreal x = luaL_checknumber( p_L, 1 );
    dsreal y = luaL_checknumber( p_L, 2 );
    dsreal z = luaL_checknumber( p_L, 3 );

    m_matrix.Translation( x, y, z );
    return 0;
}

int LuaClass_Matrix::LUA_transpose( lua_State* p_L )
{
    m_matrix.Transpose();
    return 0;
}

int LuaClass_Matrix::LUA_perspective( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 4 )
	{		
        LUA_ERROR( "Matrix::perspective : argument(s) missing" );
	}

    dsreal w = luaL_checknumber( p_L, 1 );
    dsreal h = luaL_checknumber( p_L, 2 );
    dsreal zn = luaL_checknumber( p_L, 3 );
    dsreal zf = luaL_checknumber( p_L, 4 );

    m_matrix.Perspective( w, h, zn, zf );
    return 0;
}

int LuaClass_Matrix::LUA_scale( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{		
        LUA_ERROR( "Matrix::scale : argument(s) missing" );
	}

    dsreal x = luaL_checknumber( p_L, 1 );
    dsreal y = luaL_checknumber( p_L, 2 );
    dsreal z = luaL_checknumber( p_L, 3 );

    m_matrix.Scale( x, y, z );
    return 0;
}

int LuaClass_Matrix::LUA_cleartranslation( lua_State* p_L )
{
    m_matrix.ClearTranslation();
    return 0;
}

int LuaClass_Matrix::LUA_rotation( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 4 )
	{		
        LUA_ERROR( "Matrix::rotation : argument(s) missing" );
	}

    dsreal x = luaL_checknumber( p_L, 1 );
    dsreal y = luaL_checknumber( p_L, 2 );
    dsreal z = luaL_checknumber( p_L, 3 );

    dsreal ang = luaL_checknumber( p_L, 4 );

    m_matrix.Rotation( Vector( x, y, z, 1.0 ), ang );

    return 0;
}

int LuaClass_Matrix::LUA_inverse( lua_State* p_L )
{
    m_matrix.Inverse();
    return 0;
}

int LuaClass_Matrix::LUA_setvalue( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{		
        LUA_ERROR( "Matrix::set_value : argument(s) missing" );
	}

    int r = luaL_checkint( p_L, 1 );
    int c = luaL_checkint( p_L, 2 );

    dsreal val = luaL_checknumber( p_L, 3 );

    m_matrix( r, c ) = val;

    return 0;
}

int LuaClass_Matrix::LUA_getvalue( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{		
        LUA_ERROR( "Matrix::get_value : argument(s) missing" );
	}

    int r = luaL_checkint( p_L, 1 );
    int c = luaL_checkint( p_L, 2 );

    lua_pushnumber( p_L, m_matrix( r, c ) );
    return 1;
}

DrawSpace::Utils::Matrix LuaClass_Matrix::GetMatrix( void ) const
{
    return m_matrix;
}