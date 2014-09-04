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
using namespace DrawSpace::Utils;

const char LuaRenderingNode::className[] = "RenderingNode";
const DrawSpace::Luna<LuaRenderingNode>::RegType LuaRenderingNode::Register[] =
{
    { "SetObject", &LuaRenderingNode::Lua_SetObject },
    { "GetObject", &LuaRenderingNode::Lua_GetObject },
    { "InstanciateObject", &LuaRenderingNode::Lua_InstanciateObject },
    { "SetTextureObject", &LuaRenderingNode::Lua_SetTextureObject },
    { "SetVertexTextureObject", &LuaRenderingNode::Lua_SetVertexTextureObject },
    { "GetTextureObject", &LuaRenderingNode::Lua_GetTextureObject },
    { "GetVertexTextureObject", &LuaRenderingNode::Lua_GetVertexTextureObject },
    { "SetMesheObject", &LuaRenderingNode::Lua_SetMesheObject },
    { "GetMesheObject", &LuaRenderingNode::Lua_GetMesheObject },
    { "SetFxObject", &LuaRenderingNode::Lua_SetFxObject },
    { "GetFxObject", &LuaRenderingNode::Lua_GetFxObject },
    { "SetOrderNumber", &LuaRenderingNode::Lua_SetFxObject },
    { "GetOrderNumber", &LuaRenderingNode::Lua_GetFxObject },
    { "AddShaderParameter", &LuaRenderingNode::Lua_AddShaderParameter },
    { "SetShaderReal", &LuaRenderingNode::Lua_SetShaderReal },
    { "SetShaderRealVector", &LuaRenderingNode::Lua_SetShaderRealVector },
    { "SetShaderBool", &LuaRenderingNode::Lua_SetShaderBool },
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

int LuaRenderingNode::Lua_SetTextureObject( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "SetTexture : refused, no associated rendering node object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetTexture : bad number of args" );
		lua_error( p_L );		
	}

    Texture* texture = (Texture*)luaL_checkinteger( p_L, 2 );
    long stage = luaL_checkinteger( p_L, 3 );
    m_renderingnode->SetTexture( texture, stage );

    return 0;
}

int LuaRenderingNode::Lua_SetVertexTextureObject( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "SetVertexTexture : refused, no associated rendering node object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetVertexTexture : bad number of args" );
		lua_error( p_L );		
	}

    Texture* texture = (Texture*)luaL_checkinteger( p_L, 2 );
    long stage = luaL_checkinteger( p_L, 3 );
    m_renderingnode->SetVertexTexture( texture, stage );

    return 0;
}

int LuaRenderingNode::Lua_SetMesheObject( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "SetMesheObject : refused, no associated rendering node object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetMesheObject : bad number of args" );
		lua_error( p_L );		
	}

    Meshe* meshe = (Meshe*)luaL_checkinteger( p_L, 2 );
    m_renderingnode->SetMeshe( meshe );

    return 0;
}

int LuaRenderingNode::Lua_GetMesheObject( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "GetMesheObject : refused, no associated rendering node object" );
		lua_error( p_L );
    }

    lua_pushunsigned( p_L, (lua_Unsigned)( m_renderingnode->GetMeshe() ) );
    return 1;
}

int LuaRenderingNode::Lua_SetFxObject( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "SetFxObject : refused, no associated rendering node object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetFxObject : bad number of args" );
		lua_error( p_L );		
	}

    Fx* fx = (Fx*)luaL_checkinteger( p_L, 2 );
    m_renderingnode->SetFx( fx );

    return 0;
}

int LuaRenderingNode::Lua_GetFxObject( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "GetFxObject : refused, no associated rendering node object" );
		lua_error( p_L );
    }

    lua_pushunsigned( p_L, (lua_Unsigned)( m_renderingnode->GetMeshe() ) );
    return 1;
}

int LuaRenderingNode::Lua_GetTextureObject( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "GetTextureObject : refused, no associated rendering node object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "GetTextureObject : bad number of args" );
		lua_error( p_L );		
	}

    long stage = luaL_checkinteger( p_L, 2 );
    lua_pushunsigned( p_L, (lua_Unsigned)( m_renderingnode->GetTexture( stage ) ) );

    return 1;
}

int LuaRenderingNode::Lua_GetVertexTextureObject( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "GetVertexTextureObject : refused, no associated rendering node object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "GetVertexTextureObject : bad number of args" );
		lua_error( p_L );		
	}

    long stage = luaL_checkinteger( p_L, 2 );
    lua_pushunsigned( p_L, (lua_Unsigned)( m_renderingnode->GetVertexTexture( stage ) ) );

    return 1;
}

int LuaRenderingNode::Lua_SetOrderNumber( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "SetOrderNumber : refused, no associated rendering node object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetOrderNumber : bad number of args" );
		lua_error( p_L );		
	}

    long order_value = luaL_checkinteger( p_L, 2 );
    m_renderingnode->SetOrderNumber( order_value );

    return 0;
}

int LuaRenderingNode::Lua_GetOrderNumber( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "GetOrderNumber : refused, no associated rendering node object" );
		lua_error( p_L );
    }

    lua_pushunsigned( p_L, (lua_Unsigned)( m_renderingnode->GetOrderNumber() ) );
    return 1;
}

int LuaRenderingNode::Lua_AddShaderParameter( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "AddShaderParameter : refused, no associated rendering node object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 4 )
	{
		lua_pushstring( p_L, "AddShaderParameter : bad number of args" );
		lua_error( p_L );		
	}

    long shader_index = luaL_checkinteger( p_L, 2 );
    dsstring id = luaL_checkstring( p_L, 3 );
    long shader_register = luaL_checkinteger( p_L, 4 );

    m_renderingnode->AddShaderParameter( shader_index, id, shader_register );

    return 0;
}

int LuaRenderingNode::Lua_SetShaderReal( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "SetShaderReal : refused, no associated rendering node object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetShaderReal : bad number of args" );
		lua_error( p_L );		
	}

    dsstring id = luaL_checkstring( p_L, 2 );
    dsreal val = luaL_checknumber( p_L, 3 );

    m_renderingnode->SetShaderReal( id, val );

    return 0;
}

int LuaRenderingNode::Lua_SetShaderRealVector( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "SetShaderRealVector : refused, no associated rendering node object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );

    if( 5 == argc )
    {
        dsstring id = luaL_checkstring( p_L, 2 );
        dsreal val0 = luaL_checknumber( p_L, 3 );
        dsreal val1 = luaL_checknumber( p_L, 4 );
        dsreal val2 = luaL_checknumber( p_L, 5 );

        m_renderingnode->SetShaderRealVector( id, Vector( val0, val1, val2, 1.0 ) );
    }
    else if( 3 == argc )
    {
        dsstring id = luaL_checkstring( p_L, 2 );
        Vector* vec = (Vector*)luaL_checkinteger( p_L, 3 );

        m_renderingnode->SetShaderRealVector( id, *vec );
    }
    else
    {
		lua_pushstring( p_L, "SetShaderRealVector : bad number of args" );
		lua_error( p_L );
    }

    return 0;
}

int LuaRenderingNode::Lua_SetShaderBool( lua_State* p_L )
{
    if( !m_renderingnode )
    {
		lua_pushstring( p_L, "SetShaderBool : refused, no associated rendering node object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetShaderBool : bad number of args" );
		lua_error( p_L );		
	}

    dsstring id = luaL_checkstring( p_L, 2 );
    bool val = (bool)luaL_checkinteger( p_L, 3 );

    m_renderingnode->SetShaderBool( id, val );

    return 0;
}

