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

const char LuaTransformNode::className[] = "TransformNode";
const DrawSpace::Luna<LuaTransformNode>::RegType LuaTransformNode::Register[] =
{
    { "SetObject", &LuaTransformNode::Lua_SetObject },
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


