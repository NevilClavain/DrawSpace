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

#include "lua_intermediatepass.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaIntermediatePass::className[] = "IntermediatePass";
const DrawSpace::Luna<LuaIntermediatePass>::RegType LuaIntermediatePass::Register[] =
{
    { "SetObject", &LuaIntermediatePass::Lua_SetObject },
    { "GetObject", &LuaIntermediatePass::Lua_GetObject },
    { "InstanciateObject", &LuaIntermediatePass::Lua_InstanciateObject },
    { 0 }
};

LuaIntermediatePass::LuaIntermediatePass( lua_State* p_L ) : 
m_intermediatepass( NULL ),
m_release_object( false )
{

}

LuaIntermediatePass::~LuaIntermediatePass( void ) 
{
    cleanup();
}

void LuaIntermediatePass::cleanup( void )
{
    if( m_intermediatepass && m_release_object )
    {
        _DRAWSPACE_DELETE_( m_intermediatepass );
    }
}

int LuaIntermediatePass::Lua_SetObject( lua_State* p_L )
{   
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetObject : bad number of args" );
		lua_error( p_L );		
	}

    cleanup();
    m_intermediatepass = (IntermediatePass*)luaL_checkinteger( p_L, 2 );
    m_release_object = false;

    return 0;
}

int LuaIntermediatePass::Lua_GetObject( lua_State* p_L )
{
    lua_pushunsigned( p_L, (lua_Unsigned)m_intermediatepass );
    return 1;
}

int LuaIntermediatePass::Lua_InstanciateObject( lua_State* p_L )
{
    int argc = lua_gettop( p_L );

    if( 3 == argc )
    {
        const char* id = luaL_checkstring( p_L, 2 );
        const char* name = luaL_checkstring( p_L, 3 );

        cleanup();
        m_intermediatepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( name ) );
        m_release_object = true;

        LuaBindingsDirectory::GetInstance()->Register( id, this );
    }
    else if( 5 == argc )
    {

        const char* id = luaL_checkstring( p_L, 2 );
        const char* name = luaL_checkstring( p_L, 3 );

        long target_width = (unsigned long)luaL_checkinteger( p_L, 4 );
        long target_height = (unsigned long)luaL_checkinteger( p_L, 5 );

        cleanup();
        m_intermediatepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( name, target_width, target_height ) );
        m_release_object = true;

        LuaBindingsDirectory::GetInstance()->Register( id, this );
    }
    else
    {
		lua_pushstring( p_L, "InstanciateObject : bad number of args" );
		lua_error( p_L );
    }
    return 0;
}

/*
int LuaIntermediatePass::Lua_InstanciateObject( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "InstanciateObject : bad number of args" );
		lua_error( p_L );		
	}
    
    const char* id = luaL_checkstring( p_L, 2 );
    const char* name = luaL_checkstring( p_L, 3 );

    cleanup();
    m_intermediatepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( name ) );
    m_release_object = true;

    LuaBindingsDirectory::GetInstance()->Register( id, this );
    return 0;
}

int LuaIntermediatePass::Lua_InstanciateObjectWithDims( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 5 )
	{
		lua_pushstring( p_L, "InstanciateObjectWithDims : bad number of args" );
		lua_error( p_L );		
	}
    
    const char* id = luaL_checkstring( p_L, 2 );
    const char* name = luaL_checkstring( p_L, 3 );

    long target_width = (unsigned long)luaL_checkinteger( p_L, 4 );
    long target_height = (unsigned long)luaL_checkinteger( p_L, 5 );

    cleanup();
    m_intermediatepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( name, target_width, target_height ) );
    m_release_object = true;

    LuaBindingsDirectory::GetInstance()->Register( id, this );
    return 0;
}
*/

