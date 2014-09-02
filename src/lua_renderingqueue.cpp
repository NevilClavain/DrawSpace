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
    { "InstanciateObject", &LuaRenderingQueue::Lua_AddRenderingNodeObject },
    { "InstanciateObject", &LuaRenderingQueue::Lua_Draw },
    { "InstanciateObject", &LuaRenderingQueue::Lua_EnableDepthClearing },
    { "InstanciateObject", &LuaRenderingQueue::Lua_EnableTargetClearing },
    { "InstanciateObject", &LuaRenderingQueue::Lua_SetTargetClearingColor },
    { "InstanciateObject", &LuaRenderingQueue::Lua_UpdateOuputQueue },
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
	if( argc != 3 )
	{
		lua_pushstring( p_L, "InstanciateObject : bad number of args" );
		lua_error( p_L );		
	}

    const char* id = luaL_checkstring( p_L, 2 );

    Texture* texture = (Texture*)luaL_checkinteger( p_L, 3 );

    cleanup();
    m_renderingqueue = _DRAWSPACE_NEW_( RenderingQueue, RenderingQueue( texture ) );
    m_release_object = true;

    LuaBindingsDirectory::GetInstance()->Register( id, this );
    return 0;
}

int LuaRenderingQueue::Lua_AddRenderingNodeObject( lua_State* p_L )
{
    if( !m_renderingqueue )
    {
		lua_pushstring( p_L, "AddRenderingNodeObject : refused, no associated rendering queue object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "AddRenderingNodeObject : bad number of args" );
		lua_error( p_L );		
	}

    RenderingNode* node = (RenderingNode *)luaL_checkinteger( p_L, 2 );
    m_renderingqueue->Add( node );

    return 0;
}

int LuaRenderingQueue::Lua_Draw( lua_State* p_L )
{
    if( !m_renderingqueue )
    {
		lua_pushstring( p_L, "Draw : refused, no associated rendering queue object" );
		lua_error( p_L );
    }
    m_renderingqueue->Draw();

    return 0;
}

int LuaRenderingQueue::Lua_EnableDepthClearing( lua_State* p_L )
{
    if( !m_renderingqueue )
    {
		lua_pushstring( p_L, "EnableDepthClearing : refused, no associated rendering queue object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "EnableDepthClearing : bad number of args" );
		lua_error( p_L );		
	}

    bool enable = (bool)luaL_checkinteger( p_L, 2 );
    m_renderingqueue->EnableDepthClearing( enable );

    return 0;
}

int LuaRenderingQueue::Lua_EnableTargetClearing( lua_State* p_L )
{
    if( !m_renderingqueue )
    {
		lua_pushstring( p_L, "EnableTargetClearing : refused, no associated rendering queue object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "EnableTargetClearing : bad number of args" );
		lua_error( p_L );		
	}

    bool enable = (bool)luaL_checkinteger( p_L, 2 );
    m_renderingqueue->EnableTargetClearing( enable );

    return 0;
}

int LuaRenderingQueue::Lua_SetTargetClearingColor( lua_State* p_L )
{
    if( !m_renderingqueue )
    {
		lua_pushstring( p_L, "SetTargetClearingColor : refused, no associated rendering queue object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 4 )
	{
		lua_pushstring( p_L, "SetTargetClearingColor : bad number of args" );
		lua_error( p_L );		
	}

    unsigned char r = (unsigned char)luaL_checkinteger( p_L, 2 );
    unsigned char g = (unsigned char)luaL_checkinteger( p_L, 3 );
    unsigned char b = (unsigned char)luaL_checkinteger( p_L, 4 );
    m_renderingqueue->SetTargetClearingColor( r, g, b );

    return 0;
}

int LuaRenderingQueue::Lua_UpdateOuputQueue( lua_State* p_L )
{
    if( !m_renderingqueue )
    {
		lua_pushstring( p_L, "UpdateOuputQueue : refused, no associated rendering queue object" );
		lua_error( p_L );
    }
    m_renderingqueue->UpdateOutputQueue();

    return 0;
}
