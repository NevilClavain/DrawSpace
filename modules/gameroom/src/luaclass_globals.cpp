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
    { "add_keydowncb", &LuaClass_Globals::LUA_addkeydowncb },
    { "remove_keydowncb", &LuaClass_Globals::LUA_removekeydowncb },
    { "add_keyupcb", &LuaClass_Globals::LUA_addkeyupcb },
    { "remove_keyupcb", &LuaClass_Globals::LUA_removekeyupcb },
    { "add_oncharcb", &LuaClass_Globals::LUA_addoncharcb },
    { "remove_oncharcb", &LuaClass_Globals::LUA_removeoncharcb },
    { "add_mousemovecb", &LuaClass_Globals::LUA_addmousemovecb },
    { "remove_mousemovecb", &LuaClass_Globals::LUA_removemousemovecb },
    { "add_mouseleftbuttondowncb", &LuaClass_Globals::LUA_addmouseleftbuttondowncb },
    { "remove_mouseleftbuttondowncb", &LuaClass_Globals::LUA_removemouseleftbuttondowncb },
    { "add_mouseleftbuttonupcb", &LuaClass_Globals::LUA_addmouseleftbuttonupcb },
    { "remove_mouseleftbuttonupcb", &LuaClass_Globals::LUA_removemouseleftbuttonupcb },
    { "add_mouserightbuttondowncb", &LuaClass_Globals::LUA_addmouserightbuttondowncb },
    { "remove_mouserightbuttondowncb", &LuaClass_Globals::LUA_removemouserightbuttondowncb },
    { "add_mouserightbuttonupcb", &LuaClass_Globals::LUA_addmouserightbuttonupcb },
    { "remove_mouserightbuttonupcb", &LuaClass_Globals::LUA_removemouserightbuttonupcb },

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

void LuaClass_Globals::add_callback( lua_State* p_L, const std::function<void(const std::string&, int)>& p_register_func )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
        LUA_ERROR( "Globals::add_callback : argument(s) missing" );
	}

    dsstring cbid = luaL_checkstring( p_L, 1 );
    int status = lua_isfunction( p_L, 2 );

    if( status > 0 )
    {
        int reffunc = luaL_ref( p_L, LUA_REGISTRYINDEX );     
        
        p_register_func( cbid, reffunc );
    }
    else
    {
        LUA_ERROR( "Globals::add_appruncb : argument 2 must be a function" );
    }
}

void LuaClass_Globals::remove_callback( lua_State* p_L, const std::function<int(const std::string&)>& p_unregister_func )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Globals::remove_callback : argument(s) missing" );
	}
    dsstring cbid = luaL_checkstring( p_L, 1 );

    int reffunc = p_unregister_func( cbid );
    if( -1 == reffunc )
    {
        LUA_ERROR( "Globals::remove_callback : unknown callback id" );
    }
    else
    {
        // liberer la ref...
        luaL_unref( p_L, LUA_REGISTRYINDEX, reffunc );
    }
}

int LuaClass_Globals::LUA_addappruncb( lua_State* p_L )
{
    add_callback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterRunCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removeappruncb( lua_State* p_L )
{
    remove_callback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterRunCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addkeydowncb( lua_State* p_L )
{
    add_callback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterKeyPressCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removekeydowncb( lua_State* p_L )
{
    remove_callback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterKeyPressCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addkeyupcb( lua_State* p_L )
{
    add_callback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterEndKeyPressCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removekeyupcb( lua_State* p_L )
{
    remove_callback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterEndKeyPressCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addoncharcb( lua_State* p_L )
{
    add_callback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterOnCharCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removeoncharcb( lua_State* p_L )
{
    remove_callback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterOnCharCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addmousemovecb( lua_State* p_L )
{
    add_callback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterMouseMoveCallback( p_cbid, p_reffunc ); } );
    return 0;
}
int LuaClass_Globals::LUA_removemousemovecb( lua_State* p_L )
{
    remove_callback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterMouseMoveCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addmouseleftbuttondowncb( lua_State* p_L )
{
    add_callback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterMouseLeftButtonDownCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemouseleftbuttondowncb( lua_State* p_L )
{
    remove_callback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterMouseLeftButtonDownCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addmouseleftbuttonupcb( lua_State* p_L )
{
    add_callback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterMouseLeftButtonUpCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemouseleftbuttonupcb( lua_State* p_L )
{
    remove_callback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterMouseLeftButtonUpCallback( p_cbid ); } );
    return 0;
}


int LuaClass_Globals::LUA_addmouserightbuttondowncb( lua_State* p_L )
{
    add_callback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterMouseRightButtonDownCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemouserightbuttondowncb( lua_State* p_L )
{
    remove_callback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterMouseRightButtonDownCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addmouserightbuttonupcb( lua_State* p_L )
{
    add_callback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterMouseRightButtonUpCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemouserightbuttonupcb( lua_State* p_L )
{
    remove_callback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterMouseRightButtonUpCallback( p_cbid ); } );
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