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

#include "lua_fx.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaFx::className[] = "Fx";
const DrawSpace::Luna<LuaFx>::RegType LuaFx::Register[] =
{
    { "SetObject", &LuaFx::Lua_SetObject },
    { "GetObject", &LuaFx::Lua_GetObject },
    { "InstanciateObject", &LuaFx::Lua_InstanciateObject },
    { "AddShader", &LuaFx::Lua_AddShaderObject },
    { "AddRenderStateIn", &LuaFx::Lua_AddRenderStateInObject },
    { "AddRenderStateOut", &LuaFx::Lua_AddRenderStateOutObject },
    { 0 }
};


LuaFx::LuaFx( lua_State* p_L ) : 
m_fx( NULL ),
m_release_object( false )
{

}

LuaFx::~LuaFx( void ) 
{
    cleanup();
}

void LuaFx::cleanup( void )
{
    if( m_fx && m_release_object )
    {
        _DRAWSPACE_DELETE_( m_fx );
    }
}

int LuaFx::Lua_SetObject( lua_State* p_L )
{   
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetObject : bad number of args" );
		lua_error( p_L );		
	}

    cleanup();
    m_fx = (Fx*)luaL_checkinteger( p_L, 2 );
    m_release_object = false;

    return 0;
}

int LuaFx::Lua_GetObject( lua_State* p_L )
{
    lua_pushunsigned( p_L, (lua_Unsigned)m_fx );
    return 1;
}

int LuaFx::Lua_InstanciateObject( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "InstanciateObject : bad number of args" );
		lua_error( p_L );		
	}
    
    const char* id = luaL_checkstring( p_L, 2 );

    cleanup();
    m_fx = _DRAWSPACE_NEW_( Fx, Fx );
    m_release_object = true;

    LuaBindingsDirectory::GetInstance()->Register( id, this );
    m_fx->SetName( id );
    return 0;
}

int LuaFx::Lua_AddShaderObject( lua_State* p_L )
{
    if( m_fx )
    {
	    int argc = lua_gettop( p_L );
	    if( argc != 2 )
	    {
		    lua_pushstring( p_L, "AddShaderObject : bad number of args" );
		    lua_error( p_L );		
	    }

        Shader* shader = (Shader*)luaL_checkinteger( p_L, 2 );
        m_fx->AddShader( shader );

    }
    else
    {
		lua_pushstring( p_L, "AddShaderObject : refused, no associated fx object" );
		lua_error( p_L );
    }
    return 0;
}

int LuaFx::Lua_AddRenderStateInObject( lua_State* p_L )
{
    if( m_fx )
    {
	    int argc = lua_gettop( p_L );
	    if( argc != 2 )
	    {
		    lua_pushstring( p_L, "AddRenderStateInObject : bad number of args" );
		    lua_error( p_L );		
	    }

        RenderState* rs = (RenderState*)luaL_checkinteger( p_L, 2 );
        m_fx->AddRenderStateIn( *rs );
    }
    else
    {
		lua_pushstring( p_L, "AddRenderStateInObject : refused, no associated fx object" );
		lua_error( p_L );
    }
    return 0;
}

int LuaFx::Lua_AddRenderStateOutObject( lua_State* p_L )
{
    if( m_fx )
    {
	    int argc = lua_gettop( p_L );
	    if( argc != 2 )
	    {
		    lua_pushstring( p_L, "AddRenderStateOutObject : bad number of args" );
		    lua_error( p_L );		
	    }

        RenderState* rs = (RenderState*)luaL_checkinteger( p_L, 2 );
        m_fx->AddRenderStateOut( *rs );
    }
    else
    {
		lua_pushstring( p_L, "AddRenderStateOutObject : refused, no associated fx object" );
		lua_error( p_L );
    }
    return 0;
}
