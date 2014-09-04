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

#include "lua_shader.h"

#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaShader::className[] = "Shader";
const DrawSpace::Luna<LuaShader>::RegType LuaShader::Register[] =
{
    { "SetObject", &LuaShader::Lua_SetObject },
    { "GetObject", &LuaShader::Lua_GetObject },
    { "InstanciateObject", &LuaShader::Lua_InstanciateObject },
    { "LoadFromFile", &LuaShader::Lua_LoadFromFile },
    { 0 }
};


LuaShader::LuaShader( lua_State* p_L ) : 
m_shader( NULL ),
m_release_object( false )
{

}

LuaShader::~LuaShader( void ) 
{
    cleanup();
}

void LuaShader::cleanup( void )
{
    if( m_shader && m_release_object )
    {
        _DRAWSPACE_DELETE_( m_shader );
    }
}

int LuaShader::Lua_SetObject( lua_State* p_L )
{   
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetObject : bad number of args" );
		lua_error( p_L );		
	}

    cleanup();
    m_shader = (Shader*)luaL_checkinteger( p_L, 2 );
    m_release_object = false;

    return 0;
}

int LuaShader::Lua_GetObject( lua_State* p_L )
{
    lua_pushunsigned( p_L, (lua_Unsigned)m_shader );
    return 1;
}

int LuaShader::Lua_InstanciateObject( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 4 )
	{
		lua_pushstring( p_L, "InstanciateObject : bad number of args" );
		lua_error( p_L );		
	}

    const char* id = luaL_checkstring( p_L, 2 );
    
    const char* path = luaL_checkstring( p_L, 3 );
    bool is_compiled = (bool)luaL_checkinteger( p_L, 4 );

    cleanup();
    m_shader = _DRAWSPACE_NEW_( Shader, Shader( path, is_compiled ) );
    m_release_object = true;

    LuaBindingsDirectory::GetInstance()->Register( id, this );
    m_shader->SetName( id );
    return 0;
}

int LuaShader::Lua_LoadFromFile( lua_State* p_L )
{
    bool status = false;

    if( m_shader )
    {
        status = m_shader->LoadFromFile();
    }
    else
    {
		lua_pushstring( p_L, "LoadFromFile : refused, no associated shader object" );
		lua_error( p_L );
    }

    lua_pushunsigned( p_L, (lua_Unsigned)status );
    return 1;
}
