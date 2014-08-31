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

#include "lua_renderstate.h"

#include "lua_texture.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaRenderState::className[] = "RenderState";
const DrawSpace::Luna<LuaRenderState>::RegType LuaRenderState::Register[] =
{
    { "GetObject", &LuaRenderState::Lua_GetObject },
    { "SetOperation", &LuaRenderState::Lua_SetOperation },
    { "SetArg", &LuaRenderState::Lua_SetArg },
    { 0 }
};

LuaRenderState::LuaRenderState( lua_State* p_L ) :
m_renderstate( NULL ),
m_release_object( false )
{

}

LuaRenderState::~LuaRenderState( void ) 
{
    cleanup();
}

void LuaRenderState::cleanup( void )
{
    if( m_renderstate && m_release_object )
    {
        _DRAWSPACE_DELETE_( m_renderstate );
    }
}

int LuaRenderState::Lua_SetObject( lua_State* p_L )
{   
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetObject : bad number of args" );
		lua_error( p_L );		
	}

    cleanup();
    m_renderstate = (RenderState*)luaL_checkinteger( p_L, 2 );
    m_release_object = false;

    return 0;
}


int LuaRenderState::Lua_GetObject( lua_State* p_L )
{
    lua_pushunsigned( p_L, (lua_Unsigned)m_renderstate );
    return 1;
}

int LuaRenderState::Lua_InstanciateObject( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "InstanciateObject : bad number of args" );
		lua_error( p_L );		
	}
    
    const char* id = luaL_checkstring( p_L, 2 );

    cleanup();
    m_renderstate = _DRAWSPACE_NEW_( RenderState, RenderState );
    m_release_object = true;

    LuaBindingsDirectory::GetInstance()->Register( id, this );
    return 0;
}


int LuaRenderState::Lua_SetOperation( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetOperation : bad number of args" );
		lua_error( p_L );		
	}
    
    dsstring operation( luaL_checkstring( p_L, 2 ) );

    if( "NONE" == operation )
    {
        m_renderstate->SetOperation( RenderState::NONE );
    }
    else if( "SETCLEARMASK" == operation )
    {
        m_renderstate->SetOperation( RenderState::SETCLEARMASK );
    }
    else if( "SETCLEARCOLOR" == operation )
    {
        m_renderstate->SetOperation( RenderState::SETCLEARCOLOR );
    }
    else if( "SETCULLING" == operation )
    {
        m_renderstate->SetOperation( RenderState::SETCULLING );
    }
    else if( "ENABLEZBUFFER" == operation )
    {
        m_renderstate->SetOperation( RenderState::ENABLEZBUFFER );
    }
    else if( "SETTEXTUREFILTERTYPE" == operation )
    {
        m_renderstate->SetOperation( RenderState::SETTEXTUREFILTERTYPE );
    }
    else if( "SETFILLMODE" == operation )
    {
        m_renderstate->SetOperation( RenderState::SETFILLMODE );
    }
    else if( "ALPHABLENDENABLE" == operation )
    {
        m_renderstate->SetOperation( RenderState::ALPHABLENDENABLE );
    }
    else if( "ALPHABLENDOP" == operation )
    {
        m_renderstate->SetOperation( RenderState::ALPHABLENDOP );
    }
    else if( "ALPHABLENDFUNC" == operation )
    {
        m_renderstate->SetOperation( RenderState::ALPHABLENDFUNC );
    }
    else if( "ALPHABLENDDEST" == operation )
    {
        m_renderstate->SetOperation( RenderState::ALPHABLENDDEST );
    }
    else if( "ALPHABLENDSRC" == operation )
    {
        m_renderstate->SetOperation( RenderState::ALPHABLENDSRC );
    }

    return 0;
}

int LuaRenderState::Lua_SetArg( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetOperation : bad number of args" );
		lua_error( p_L );		
	}
   
    dsstring arg( luaL_checkstring( p_L, 2 ) );

    m_renderstate->SetArg( arg );

    return 0;
}
