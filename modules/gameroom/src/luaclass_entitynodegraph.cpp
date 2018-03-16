/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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
/* -*-LIC_END-*- */

#include "luaclass_entitynodegraph.h"
#include "luaclass_entity.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaClass_EntityNodeGraph::className[] = "EntityNodeGraph";
const Luna<LuaClass_EntityNodeGraph>::RegType LuaClass_EntityNodeGraph::methods[] =
{
    { "set_root", &LuaClass_EntityNodeGraph::LUA_setroot },
	{ 0, 0 }
};

LuaClass_EntityNodeGraph::LuaClass_EntityNodeGraph( lua_State* p_L )
{
}

LuaClass_EntityNodeGraph::~LuaClass_EntityNodeGraph( void )
{
}

int LuaClass_EntityNodeGraph::LUA_setroot( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
		lua_pushstring( p_L, "EntityNodeGraph::set_root : argument(s) missing" );
		lua_error( p_L );		
	}

    dsstring entity_id = luaL_checkstring( p_L, 1 );
    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 2 );
    
    m_entities[entity_id] = m_entitygraph.SetRoot( &lua_ent->GetEntity() );

    return 0;
}