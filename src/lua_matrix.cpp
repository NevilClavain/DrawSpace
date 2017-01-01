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

#include "lua_matrix.h"
#include "lua_vector.h"
#include "maths.h"

const char LuaMatrix::className[] = "Matrix";
const Luna2<LuaMatrix>::RegType LuaMatrix::methods[] =
{
    { "Set", &LuaMatrix::Lua_Set },
    { "Get", &LuaMatrix::Lua_Get },
    { "Zero", &LuaMatrix::Lua_Zero },
    { "Identity", &LuaMatrix::Lua_Identity },
    { "Translation", &LuaMatrix::Lua_Translation },
    { "Transpose", &LuaMatrix::Lua_Transpose },
    { "Perspective", &LuaMatrix::Lua_Perspective },
    { "Scale", &LuaMatrix::Lua_Scale },
    { "ClearTranslation", &LuaMatrix::Lua_ClearTranslation },
    { "Rotation", &LuaMatrix::Lua_Rotation },
    { "Inverse", &LuaMatrix::Lua_Inverse },
    { "Transform", &LuaMatrix::Lua_Transform },
    { 0, 0 }
};

LuaMatrix::LuaMatrix( lua_State* p_L )
{
}

LuaMatrix::~LuaMatrix( void ) 
{
}

int LuaMatrix::Lua_Set( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "Matrix Set : bad number of args" );
		lua_error( p_L );		
    }

    int index_row = luaL_checkinteger( p_L, 1 );
    if( index_row > 3 ) index_row = 3;

    int index_col = luaL_checkinteger( p_L, 2 );
    if( index_col > 3 ) index_col = 3;

    dsreal val = luaL_checknumber( p_L, 3 );

    m_mat( index_row, index_col ) = val;
    return 0;
}

int LuaMatrix::Lua_Get( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "Matrix Get : bad number of args" );
		lua_error( p_L );
    }

    int index_row = luaL_checkinteger( p_L, 1 );
    if( index_row > 3 ) index_row = 3;

    int index_col = luaL_checkinteger( p_L, 2 );
    if( index_col > 3 ) index_col = 3;

    lua_pushnumber( p_L, m_mat( index_row, index_col ) );
    return 1;
}

int LuaMatrix::Lua_Zero( lua_State* p_L )
{
    m_mat.Zero();
    return 0;
}

int LuaMatrix::Lua_Identity( lua_State* p_L )
{
    m_mat.Identity();
    return 0;
}

int LuaMatrix::Lua_Translation( lua_State* p_L )
{
    int argc = lua_gettop( p_L );
    if( 1 == argc )
    {
        LuaVector* vec = Luna2<LuaVector>::check( p_L, 1 );

        m_mat.Translation( vec->m_vector );
    }
    else if( 3 == argc )
    {
        dsreal valx = luaL_checknumber( p_L, 1 );
        dsreal valy = luaL_checknumber( p_L, 2 );
        dsreal valz = luaL_checknumber( p_L, 3 );

        m_mat.Translation( valx, valy, valz );
    }
    else
    {
		lua_pushstring( p_L, "Matrix Translation : bad number of args" );
		lua_error( p_L );
    }
    return 0;
}

int LuaMatrix::Lua_Transpose( lua_State* p_L )
{
    m_mat.Transpose();
    return 0;
}

int LuaMatrix::Lua_Perspective( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 4 )
	{
		lua_pushstring( p_L, "Matrix Perspective : bad number of args" );
		lua_error( p_L );
    }

    dsreal w = luaL_checknumber( p_L, 1 );
    dsreal h = luaL_checknumber( p_L, 2 );
    dsreal zn = luaL_checknumber( p_L, 3 );
    dsreal zf = luaL_checknumber( p_L, 3 );

    m_mat.Perspective( w, h, zn, zf );
    return 0;
}

int LuaMatrix::Lua_Scale( lua_State* p_L )
{
	int argc = lua_gettop( p_L );

    if( 1 == argc )
    {
        LuaVector* vec = Luna2<LuaVector>::check( p_L, 1 );

        m_mat.Scale( vec->m_vector );
    }
    else if( 3 == argc )
    {
        dsreal valx = luaL_checknumber( p_L, 1 );
        dsreal valy = luaL_checknumber( p_L, 2 );
        dsreal valz = luaL_checknumber( p_L, 3 );

        m_mat.Scale( valx, valy, valz );
    }
    else
    {
		lua_pushstring( p_L, "Matrix Scale : bad number of args" );
		lua_error( p_L );
    }
    return 0;
}

int LuaMatrix::Lua_ClearTranslation( lua_State* p_L )
{
    m_mat.ClearTranslation();
    return 0;
}

int LuaMatrix::Lua_Rotation( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "Matrix Rotation : bad number of args" );
		lua_error( p_L );
    }

    LuaVector* vec_axis = Luna2<LuaVector>::check( p_L, 1 );
    dsreal angle = luaL_checknumber( p_L, 2 );

    m_mat.Rotation( vec_axis->m_vector, DrawSpace::Utils::Maths::DegToRad( angle ) );
    return 0;
}

int LuaMatrix::Lua_Inverse( lua_State* p_L )
{
    m_mat.Inverse();
    return 0;
}

int LuaMatrix::Lua_Transform( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "Matrix Transform : bad number of args" );
		lua_error( p_L );
    }

    LuaVector* vec_source = Luna2<LuaVector>::check( p_L, 1 );
    LuaVector* vec_dest = Luna2<LuaVector>::check( p_L, 2 );

    m_mat.Transform( &vec_source->m_vector, &vec_dest->m_vector );
    return 0;
}

