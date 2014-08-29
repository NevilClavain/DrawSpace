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

#include "lua_viewportquad.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaViewportQuad::className[] = "ViewportQuad";
const DrawSpace::Luna<LuaViewportQuad>::RegType LuaViewportQuad::Register[] =
{
    { "SetObject", &LuaViewportQuad::Lua_SetObject },
    { "GetObject", &LuaViewportQuad::Lua_GetObject },
    { "InstanciateObject", &LuaViewportQuad::Lua_InstanciateObject },
    { 0 }
};


LuaViewportQuad::LuaViewportQuad( lua_State* p_L ) : 
m_viewportquad( NULL ),
m_release_object( false )
{

}

LuaViewportQuad::~LuaViewportQuad( void ) 
{
    cleanup();
}

void LuaViewportQuad::cleanup( void )
{
    if( m_viewportquad && m_release_object )
    {
        _DRAWSPACE_DELETE_( m_viewportquad );
    }
}

int LuaViewportQuad::Lua_SetObject( lua_State* p_L )
{   
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetObject : bad number of args" );
		lua_error( p_L );		
	}

    cleanup();
    m_viewportquad = (ViewportQuad*)luaL_checkinteger( p_L, 2 );
    m_release_object = false;

    return 0;
}

int LuaViewportQuad::Lua_GetObject( lua_State* p_L )
{
    lua_pushunsigned( p_L, (lua_Unsigned)m_viewportquad );
    return 1;
}

int LuaViewportQuad::Lua_InstanciateObject( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 4 )
	{
		lua_pushstring( p_L, "InstanciateObject : bad number of args" );
		lua_error( p_L );		
	}
    
    const char* name = luaL_checkstring( p_L, 2 );

    dsreal width = luaL_checknumber( p_L, 3 );
    dsreal height = luaL_checknumber( p_L, 4 );

    cleanup();
    m_viewportquad = _DRAWSPACE_NEW_( ViewportQuad, ViewportQuad( name, width, height ) );
    m_release_object = true;

    return 0;
}

