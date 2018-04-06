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

#include "luacontext.h"
#include "luaclass_globals.h"
#include "mainservice.h"
#include "memalloc.h"

const char LuaClass_Globals::className[] = "Globals";
const Luna<LuaClass_Globals>::RegType LuaClass_Globals::methods[] =
{
    { "quit", &LuaClass_Globals::LUA_quit },
    { "clear_console", &LuaClass_Globals::LUA_clearconsole },
    { "print", &LuaClass_Globals::LUA_print },
    { "dofile", &LuaClass_Globals::LUA_dofile },
    { "dumpmem", &LuaClass_Globals::LUA_dumpmem },
    { "totalmem", &LuaClass_Globals::LUA_totalmem },
    { "add_appruncb", &LuaClass_Globals::LUA_addappruncb },
    { "remove_appruncb", &LuaClass_Globals::LUA_removeappruncb },
    { "reset", &LuaClass_Globals::LUA_reset },
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
        LUA_ERROR( "Globals::print : argument(s) missing" );
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
        LUA_ERROR( "Globals::dofile : argument(s) missing" );
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

int LuaClass_Globals::LUA_addappruncb( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
        LUA_ERROR( "Globals::add_appruncb : argument(s) missing" );
	}

    dsstring cbid = luaL_checkstring( p_L, 1 );
    int status = lua_isfunction( p_L, 2 );

    if( status > 0 )
    {
        int reffunc = luaL_ref( p_L, LUA_REGISTRYINDEX );     
        MainService::GetInstance()->RegisterRunCallback( cbid, reffunc );
    }
    else
    {
        LUA_ERROR( "Globals::add_appruncb : argument 2 must be a function" );
    }

    return 0;
}

int LuaClass_Globals::LUA_removeappruncb( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Globals::remove_appruncb : argument(s) missing" );
	}
    dsstring cbid = luaL_checkstring( p_L, 1 );

    int reffunc = MainService::GetInstance()->UnregisterRunCallback( cbid );

    if( -1 == reffunc )
    {
        LUA_ERROR( "Globals::remove_appruncb : unknown callback id" );
    }
    else
    {
        // liberer la ref...
        luaL_unref( p_L, LUA_REGISTRYINDEX, reffunc );
    }
    return 0;
}

int LuaClass_Globals::LUA_totalmem( lua_State* p_L )
{
    lua_pushinteger( p_L, DrawSpace::Utils::MemAlloc::GetInstance()->GetTotalSize() );
    return 1;
}

int LuaClass_Globals::LUA_reset( lua_State* p_L )
{
    MainService::GetInstance()->RequestLuaStackReset();
    return 0;
}