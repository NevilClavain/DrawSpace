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

#include "luaclass_globals.h"
#include "mainservice.h"

const char LuaClass_Globals::className[] = "Globals";
const Luna<LuaClass_Globals>::RegType LuaClass_Globals::methods[] =
{
    { "quit", &LuaClass_Globals::LUA_quit },
    { "clear_console", &LuaClass_Globals::LUA_clearconsole },
    { "print", &LuaClass_Globals::LUA_print },
    { "dofile", &LuaClass_Globals::LUA_dofile },
    { "dumpmem", &LuaClass_Globals::LUA_dumpmem },
	{ 0, 0 }
};

LuaClass_Globals::LuaClass_Globals( lua_State* p_L )
{
}

LuaClass_Globals::~LuaClass_Globals( void )
{
}

int LuaClass_Globals::LUA_quit( lua_State* p_L )
{
    MainService::GetInstance()->RequestClose();
    return 0;
}

int LuaClass_Globals::LUA_clearconsole( lua_State* p_L )
{
    MainService::GetInstance()->RequestClearConsole();
    return 0;
}

int LuaClass_Globals::LUA_print( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "Globals::print : argument(s) missing" );
		lua_error( p_L );		
	}

	dsstring msg = luaL_checkstring( p_L, 1 );

    MainService::GetInstance()->RequestConsolePrint( msg );
    return 0;
}

int LuaClass_Globals::LUA_dofile( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "Globals::dofile : argument(s) missing" );
		lua_error( p_L );		
	}

	dsstring path = luaL_checkstring( p_L, 1 );

    MainService::GetInstance()->RequestLuaFileExec( path );

    return 0;
}

int LuaClass_Globals::LUA_dumpmem( lua_State* p_L )
{
    MainService::GetInstance()->RequestMemAllocDump();
    return 0;
}