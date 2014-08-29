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

#include "lua_scenegraph.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaScenegraph::className[] = "Scenegraph";
const DrawSpace::Luna<LuaScenegraph>::RegType LuaScenegraph::Register[] =
{
    { "SetObject", &LuaScenegraph::Lua_SetObject },
    { "GetObject", &LuaScenegraph::Lua_GetObject },
    { "InstanciateObject", &LuaScenegraph::Lua_InstanciateObject },
    { 0 }
};


LuaScenegraph::LuaScenegraph( lua_State* p_L ) : 
m_scenegraph( NULL ),
m_release_object( false )
{

}

LuaScenegraph::~LuaScenegraph( void ) 
{
    cleanup();
}

void LuaScenegraph::cleanup( void )
{
    if( m_scenegraph && m_release_object )
    {
        _DRAWSPACE_DELETE_( m_scenegraph );
    }
}

int LuaScenegraph::Lua_SetObject( lua_State* p_L )
{   
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetObject : bad number of args" );
		lua_error( p_L );		
	}

    cleanup();
    m_scenegraph = (Scenegraph*)luaL_checkinteger( p_L, 2 );
    m_release_object = false;

    return 0;
}

int LuaScenegraph::Lua_GetObject( lua_State* p_L )
{
    lua_pushunsigned( p_L, (lua_Unsigned)m_scenegraph );
    return 1;
}

int LuaScenegraph::Lua_InstanciateObject( lua_State* p_L )
{

    cleanup();
    m_scenegraph = _DRAWSPACE_NEW_( Scenegraph, Scenegraph );
    m_release_object = true;

    return 0;
}

