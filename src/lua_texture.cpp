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

#include "lua_texture.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaTexture::className[] = "Texture";
const DrawSpace::Luna<LuaTexture>::RegType LuaTexture::Register[] =
{
    { "SetObject", &LuaTexture::Lua_SetObject },
    { "GetObject", &LuaTexture::Lua_GetObject },
    { "InstanciateObject", &LuaTexture::Lua_InstanciateObject },
    { "LoadFromFile", &LuaTexture::Lua_LoadFromFile },
    { 0 }
};


LuaTexture::LuaTexture( lua_State* p_L ) : 
m_texture( NULL ),
m_release_object( false )
{

}

LuaTexture::~LuaTexture( void ) 
{
    cleanup();
}

void LuaTexture::cleanup( void )
{
    if( m_texture && m_release_object )
    {
        _DRAWSPACE_DELETE_( m_texture );
    }
}

int LuaTexture::Lua_SetObject( lua_State* p_L )
{   
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetObject : bad number of args" );
		lua_error( p_L );		
	}

    cleanup();
    m_texture = (Texture*)luaL_checkinteger( p_L, 2 );
    m_release_object = false;

    return 0;
}

int LuaTexture::Lua_GetObject( lua_State* p_L )
{
    lua_pushunsigned( p_L, (lua_Unsigned)m_texture );
    return 1;
}

int LuaTexture::Lua_InstanciateObject( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 6 && argc != 3 )
	{
		lua_pushstring( p_L, "InstanciateObject : bad number of args" );
		lua_error( p_L );		
	}
    
    const char* id = luaL_checkstring( p_L, 2 );

    const char* path = luaL_checkstring( p_L, 3 );

    if( 6 == argc )
    {
        bool render_target = (bool)luaL_checkinteger( p_L, 4 );
        unsigned long render_target_width = (unsigned long)luaL_checkinteger( p_L, 5 );
        unsigned long render_target_height = (unsigned long)luaL_checkinteger( p_L, 6 );

        cleanup();
        m_texture = _DRAWSPACE_NEW_( Texture, Texture( path, render_target, render_target_width, render_target_height ) );
    }
    else
    {
        cleanup();
        m_texture = _DRAWSPACE_NEW_( Texture, Texture( path ) );
    }

    m_release_object = true;

    LuaBindingsDirectory::GetInstance()->Register( id, this );
    m_texture->SetName( id );
    return 0;
}

int LuaTexture::Lua_LoadFromFile( lua_State* p_L )
{
    bool status = false;

    if( m_texture )
    {
        status = m_texture->LoadFromFile();
    }
    else
    {
		lua_pushstring( p_L, "LoadFromFile : refused, no associated texture object" );
		lua_error( p_L );
    }

    lua_pushunsigned( p_L, (lua_Unsigned)status );
    return 1;
}

