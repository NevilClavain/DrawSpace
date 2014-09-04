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

#include "lua_transformnode.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

const char LuaTransformNode::className[] = "TransformNode";
const DrawSpace::Luna<LuaTransformNode>::RegType LuaTransformNode::Register[] =
{
    { "SetObject", &LuaTransformNode::Lua_SetObject },
    { "SetName", &LuaTransformNode::Lua_SetName },
    { "GetName", &LuaTransformNode::Lua_GetName },
    { "SetLocalTransform", &LuaTransformNode::Lua_SetLocalTransform },
    { 0 }
};


LuaTransformNode::LuaTransformNode( lua_State* p_L ) : 
m_transformnode( NULL )
{

}

LuaTransformNode::~LuaTransformNode( void ) 
{
}


int LuaTransformNode::Lua_SetObject( lua_State* p_L )
{   
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetObject : bad number of args" );
		lua_error( p_L );		
	}
    m_transformnode = (TransformNode*)luaL_checkinteger( p_L, 2 );

    return 0;
}

int LuaTransformNode::Lua_SetName( lua_State* p_L )
{
    if( !m_transformnode )
    {
		lua_pushstring( p_L, "SetName : refused, no associated transform node object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetName : bad number of args" );
		lua_error( p_L );		
	}

    const char* name = luaL_checkstring( p_L, 2 );
    m_transformnode->SetName( dsstring( name ) );

    return 0;
}

int LuaTransformNode::Lua_GetName( lua_State* p_L )
{
    if( !m_transformnode )
    {
		lua_pushstring( p_L, "GetName : refused, no associated transform node object" );
		lua_error( p_L );
    }

    dsstring name;
    m_transformnode->GetName( name );

    lua_pushstring( p_L, name.c_str() );
    return 1;
}

int LuaTransformNode::Lua_SetLocalTransform( lua_State* p_L )
{
    if( !m_transformnode )
    {
		lua_pushstring( p_L, "SetLocalTransform : refused, no associated transform node object" );
		lua_error( p_L );
    }

	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetLocalTransform : bad number of args" );
		lua_error( p_L );		
	}

    Matrix* matrix = (Matrix*)luaL_checkinteger( p_L, 2 );
    m_transformnode->SetLocalTransform( *matrix );

    return 0;
}

