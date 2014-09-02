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

#include "lua_renderingnode.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaRenderingNode::className[] = "RenderingNode";
const DrawSpace::Luna<LuaRenderingNode>::RegType LuaRenderingNode::Register[] =
{
    { "SetObject", &LuaRenderingNode::Lua_SetObject },
    { "GetObject", &LuaRenderingNode::Lua_GetObject },
    { "InstanciateObject", &LuaRenderingNode::Lua_InstanciateObject },
    { 0 }
};


LuaRenderingNode::LuaRenderingNode( lua_State* p_L ) : 
m_renderingnode( NULL ),
m_release_object( false )
{

}

LuaRenderingNode::~LuaRenderingNode( void ) 
{
    cleanup();
}

void LuaRenderingNode::cleanup( void )
{
    if( m_renderingnode && m_release_object )
    {
        _DRAWSPACE_DELETE_( m_renderingnode );
    }
}

int LuaRenderingNode::Lua_SetObject( lua_State* p_L )
{   
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetObject : bad number of args" );
		lua_error( p_L );		
	}

    cleanup();
    m_renderingnode = (RenderingNode*)luaL_checkinteger( p_L, 2 );
    m_release_object = false;

    return 0;
}

int LuaRenderingNode::Lua_GetObject( lua_State* p_L )
{
    lua_pushunsigned( p_L, (lua_Unsigned)m_renderingnode );
    return 1;
}

int LuaRenderingNode::Lua_InstanciateObject( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "InstanciateObject : bad number of args" );
		lua_error( p_L );		
	}
    
    const char* id = luaL_checkstring( p_L, 2 );

    cleanup();
    m_renderingnode = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );
    m_release_object = true;

    LuaBindingsDirectory::GetInstance()->Register( id, this );
    return 0;
}
