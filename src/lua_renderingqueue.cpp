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

#include "lua_renderingqueue.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaRenderingQueue::className[] = "RenderingQueue";
const DrawSpace::Luna<LuaRenderingQueue>::RegType LuaRenderingQueue::Register[] =
{
    { "SetObject", &LuaRenderingQueue::Lua_SetObject },
    { "GetObject", &LuaRenderingQueue::Lua_GetObject },
    { "InstanciateObject", &LuaRenderingQueue::Lua_InstanciateObject },
    { 0 }
};


LuaRenderingQueue::LuaRenderingQueue( lua_State* p_L ) : 
m_renderingqueue( NULL ),
m_release_object( false )
{

}

LuaRenderingQueue::~LuaRenderingQueue( void ) 
{
    cleanup();
}

void LuaRenderingQueue::cleanup( void )
{
    if( m_renderingqueue && m_release_object )
    {
        _DRAWSPACE_DELETE_( m_renderingqueue );
    }
}

int LuaRenderingQueue::Lua_SetObject( lua_State* p_L )
{   
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetObject : bad number of args" );
		lua_error( p_L );		
	}

    cleanup();
    m_renderingqueue = (RenderingQueue*)luaL_checkinteger( p_L, 2 );
    m_release_object = false;

    return 0;
}

int LuaRenderingQueue::Lua_GetObject( lua_State* p_L )
{
    lua_pushunsigned( p_L, (lua_Unsigned)m_release_object );
    return 1;
}

int LuaRenderingQueue::Lua_InstanciateObject( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "InstanciateObject : bad number of args" );
		lua_error( p_L );		
	}

    Texture* texture = (Texture*)luaL_checkinteger( p_L, 2 );

    cleanup();
    m_renderingqueue = _DRAWSPACE_NEW_( RenderingQueue, RenderingQueue( texture ) );
    m_release_object = true;

    return 0;
}

