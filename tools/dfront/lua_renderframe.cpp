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

#include "lua_renderframe.h"


const char LuaRenderFrame::className[] = "RenderFrame";
const DrawSpace::Luna<LuaRenderFrame>::RegType LuaRenderFrame::Register[] =
{
  { "SetCurrentScene", &LuaRenderFrame::Lua_SetCurrentScene },
  { "InstanciateScene", &LuaRenderFrame::Lua_InstanciateScene },
  { "GetScene", &LuaRenderFrame::Lua_GetScene },
  { 0 }
};


LuaRenderFrame::LuaRenderFrame( lua_State* p_L ) 
{
    m_instance = RenderFrame::GetInstance();    
}

LuaRenderFrame::~LuaRenderFrame( void ) 
{
}

int LuaRenderFrame::Lua_SetCurrentScene( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetCurrentScene : bad number of args" );
		lua_error( p_L );		
	}

    const char* scene_name = luaL_checklstring( p_L, 2, NULL );

    bool status = m_instance->SetCurrentScene( dsstring( scene_name ) );

    if( false == status )
    {
		lua_pushstring( p_L, "SetCurrentScene : unknown scene" );
		lua_error( p_L );
    }

    return 0;
}

int LuaRenderFrame::Lua_InstanciateScene( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "InstanciateScene : bad number of args" );
		lua_error( p_L );		
	}

    const char* scene_name = luaL_checklstring( p_L, 2, NULL );

    m_instance->InstanciateScene( scene_name );

    return 0;
}

int LuaRenderFrame::Lua_GetScene( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "GetScene : bad number of args" );
		lua_error( p_L );		
	}

    const char* scene_name = luaL_checklstring( p_L, 2, NULL );
    Scene* scene = m_instance->GetScene( dsstring( scene_name ) );
    if( NULL == scene )
    {
		lua_pushstring( p_L, "GetScene : unknown scene" );
		lua_error( p_L );        
    }

	lua_newtable( p_L );
	lua_pushnumber( p_L, 0 );

	Scene** a = (Scene**)lua_newuserdata(p_L, sizeof(Scene*));

	*a = scene;
	luaL_getmetatable(p_L, Scene::className);
	lua_setmetatable(p_L, -2);
	lua_settable(p_L, -3);
	for ( int i = 0; Scene::Register[i].name; i++ )
	{
		lua_pushstring(p_L, Scene::Register[i].name);
		lua_pushnumber(p_L, i);
        lua_pushcclosure(p_L, &DrawSpace::Luna<Scene>::thunk, 1);
		lua_settable(p_L, -3);
	}
	return 1;
}
