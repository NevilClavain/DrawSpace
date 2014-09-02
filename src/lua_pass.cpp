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

#include "lua_pass.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaPass::className[] = "Pass";
const DrawSpace::Luna<LuaPass>::RegType LuaPass::Register[] =
{
    { "SetObject", &LuaPass::Lua_SetObject },
    { "GetRenderingQueueObject", &LuaPass::Lua_GetRenderingQueueObject },
    { "CreateViewportQuad", &LuaPass::Lua_CreateViewportQuad },
    { "GetViewportQuadObject", &LuaPass::Lua_GetViewportQuadObject },
    { "GetName", &LuaPass::Lua_GetName },
    { 0 }
};


LuaPass::LuaPass( lua_State* p_L ) : 
m_pass( NULL )
{
}

LuaPass::~LuaPass( void ) 
{
}

int LuaPass::Lua_SetObject( lua_State* p_L )
{   
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetObject : bad number of args" );
		lua_error( p_L );		
	}

    m_pass = (Pass*)luaL_checkinteger( p_L, 2 );
    return 0;
}

int LuaPass::Lua_GetRenderingQueueObject( lua_State* p_L )
{
    if( !m_pass )
    {
		lua_pushstring( p_L, "GetRenderingQueueObject : refused, no associated pass object" );
		lua_error( p_L );
    }
    lua_pushunsigned( p_L, (lua_Unsigned)m_pass->GetRenderingQueue() );

    return 1;
}

int LuaPass::Lua_CreateViewportQuad( lua_State* p_L )
{
    if( !m_pass )
    {
		lua_pushstring( p_L, "CreateViewportQuad : refused, no associated pass object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );

    if( 1 == argc )
    {
        m_pass->CreateViewportQuad();
    }
    else if( 3 == argc )
    {
        dsreal width = luaL_checknumber( p_L, 2 );
        dsreal height = luaL_checknumber( p_L, 3 );

        m_pass->CreateViewportQuad( width, height );
    }
    else
    {
		lua_pushstring( p_L, "CreateViewportQuad : bad number of args" );
		lua_error( p_L );
    }
    return 0;
}

int LuaPass::Lua_GetViewportQuadObject( lua_State* p_L )
{
    if( !m_pass )
    {
		lua_pushstring( p_L, "GetViewportQuadObject : refused, no associated pass object" );
		lua_error( p_L );
    }
    lua_pushunsigned( p_L, (lua_Unsigned)m_pass->GetViewportQuad() );

    return 1;
}

int LuaPass::Lua_GetName( lua_State* p_L )
{
    if( !m_pass )
    {
		lua_pushstring( p_L, "GetName : refused, no associated pass object" );
		lua_error( p_L );
    }

    dsstring name;
    m_pass->GetName( name );

    lua_pushstring( p_L, name.c_str() );
    return 1;
}