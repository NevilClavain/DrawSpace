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
const Luna<LuaRenderFrame>::RegType LuaRenderFrame::Register[] =
{
  { "SetCurrentScene", &LuaRenderFrame::Lua_SetCurrentScene },
  { "GetMesheImportObject", &LuaRenderFrame::Lua_GetMesheImportObject },
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

    Scene* scene = (Scene*)luaL_checkinteger( p_L, 2 );

    bool status = m_instance->SetCurrentScene( scene );
    if( false == status )
    {
		lua_pushstring( p_L, "SetCurrentScene : refused because specified scene has non name" );
		lua_error( p_L );
    }
    return 0;
}

int LuaRenderFrame::Lua_GetMesheImportObject( lua_State* p_L )
{
    lua_pushunsigned( p_L, (lua_Unsigned)m_instance->m_meshe_import );
    return 0;
}
