/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#include <iomanip> // setprecision
#include <sstream> // stringstream

#include "luacontext.h"
#include "luaclass_globals.h"
#include "mainservice.h"
#include "memalloc.h"
#include "shader.h"
#include "texture.h"
#include "maths.h"

#include "luaclass_matrix.h"
#include "luaclass_vector.h"
#include "luaclass_quaternion.h"

#include "luaext_load.h"


using namespace DrawSpace;
using namespace DrawSpace::Commons;

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


const char LuaClass_Globals::className[] = "Globals";
const Luna<LuaClass_Globals>::RegType LuaClass_Globals::methods[] =
{
    { "quit", &LuaClass_Globals::LUA_quit },
    { "clear_console", &LuaClass_Globals::LUA_clearconsole },
    { "print", &LuaClass_Globals::LUA_print },
    { "do_file", &LuaClass_Globals::LUA_dofile },
    { "dump_mem", &LuaClass_Globals::LUA_dumpmem },
    { "total_mem", &LuaClass_Globals::LUA_totalmem },
    { "log", &LuaClass_Globals::LUA_log },
    { "format_real", &LuaClass_Globals::LUA_formatreal },
	{ "breakpoint", &LuaClass_Globals::LUA_breakpoint },

    { "round", &LuaClass_Globals::LUA_round },
    { "pow", &LuaClass_Globals::LUA_pow },
    { "clamp", &LuaClass_Globals::LUA_clamp },
	{ "stoi", &LuaClass_Globals::LUA_stoi },
    { "cos", &LuaClass_Globals::LUA_cos },
    { "sin", &LuaClass_Globals::LUA_sin },
    { "ctos", &LuaClass_Globals::LUA_ctos },
    { "stoc", &LuaClass_Globals::LUA_stoc },
    { "rotate_quaternion_x", &LuaClass_Globals::LUA_rotatequaternionx },

    
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
	{ "add_animationeventcb", &LuaClass_Globals::LUA_addanimationeventcb },
	{ "remove_animationeventcb", &LuaClass_Globals::LUA_removeanimationeventcb },
    { "add_resourceeventcb", &LuaClass_Globals::LUA_addresourceeventcb },
    { "remove_resourceeventcb", &LuaClass_Globals::LUA_removeresourceeventcb },

    { "show_mousecursor", &LuaClass_Globals::LUA_showmousecursor },
    { "set_mousecursorcircularmode", &LuaClass_Globals::LUA_setmousecursorcircularmode },

    { "set_texturesrootpath", &LuaClass_Globals::LUA_settexturesrootpath },
    { "set_shadersrootpath", &LuaClass_Globals::LUA_setshadersrootpath },
    { "enable_shadersdescrinfinalpath", &LuaClass_Globals::LUA_enableshadersdescrinfinalpath },
    { "set_meshesrootpath", &LuaClass_Globals::LUA_setmeshesrootpath },
    { "set_scriptsrootpath", &LuaClass_Globals::LUA_setscriptsrootpath },
    { "set_virtualfs", &LuaClass_Globals::LUA_setvirtualfs },

    { "sleep", &LuaClass_Globals::LUA_sleep },
    { "reset", &LuaClass_Globals::LUA_reset },

    { "signal_renderscenebegin", &LuaClass_Globals::LUA_signalrenderscenebegin },
    { "signal_rendersceneend", &LuaClass_Globals::LUA_signalrendersceneend },

    { "ds_exception", &LuaClass_Globals::LUA_dsexception },

    { "release_assets", &LuaClass_Globals::LUA_releaseassets },

    { "activate_resourcessystem", &LuaClass_Globals::LUA_activateresourcessystem },
    { "deactivate_resourcessystem", &LuaClass_Globals::LUA_deactivateresourcessystem },

    { "register_extension", &LuaClass_Globals::LUA_registerextension },
    { "dump_allextensionsalloc", &LuaClass_Globals::LUA_dumpallextensionsalloc },
    { "get_extensionsalloctotalsize", &LuaClass_Globals::LUA_getextensionsalloctotalsize },

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
    MainService::getInstance()->RequestClose();
    return 0;
}

int LuaClass_Globals::LUA_clearconsole( lua_State* p_L )
{
    MainService::getInstance()->RequestClearConsole();
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

    MainService::getInstance()->RequestConsolePrint( msg );
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
	MainService::getInstance()->RequestLuaFileExec(path);

    return 0;
}

int LuaClass_Globals::LUA_dumpmem( lua_State* p_L )
{
    MainService::getInstance()->RequestMemAllocDump();
    return 0;
}

int LuaClass_Globals::LUA_addappruncb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::getInstance()->RegisterRunCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removeappruncb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::getInstance()->UnregisterRunCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addkeydowncb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::getInstance()->RegisterKeyPressCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removekeydowncb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::getInstance()->UnregisterKeyPressCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addkeyupcb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::getInstance()->RegisterEndKeyPressCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removekeyupcb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::getInstance()->UnregisterEndKeyPressCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addoncharcb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::getInstance()->RegisterOnCharCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removeoncharcb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::getInstance()->UnregisterOnCharCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addmousemovecb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::getInstance()->RegisterMouseMoveCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemousemovecb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::getInstance()->UnregisterMouseMoveCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addmouseleftbuttondowncb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::getInstance()->RegisterMouseLeftButtonDownCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemouseleftbuttondowncb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::getInstance()->UnregisterMouseLeftButtonDownCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addmouseleftbuttonupcb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::getInstance()->RegisterMouseLeftButtonUpCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemouseleftbuttonupcb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::getInstance()->UnregisterMouseLeftButtonUpCallback( p_cbid ); } );
    return 0;
}


int LuaClass_Globals::LUA_addmouserightbuttondowncb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::getInstance()->RegisterMouseRightButtonDownCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemouserightbuttondowncb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::getInstance()->UnregisterMouseRightButtonDownCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addmouserightbuttonupcb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::getInstance()->RegisterMouseRightButtonUpCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemouserightbuttonupcb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::getInstance()->UnregisterMouseRightButtonUpCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addanimationeventcb(lua_State* p_L)
{
	LuaContext::AddCallback(p_L, [](const std::string& p_cbid, int p_reffunc) { MainService::getInstance()->RegisterAnimationEventCallback(p_cbid, p_reffunc); });
	return 0;
}

int LuaClass_Globals::LUA_removeanimationeventcb(lua_State* p_L)
{
	LuaContext::RemoveCallback(p_L, [](const std::string& p_cbid)->int { return MainService::getInstance()->UnregisterAnimationEventCallback(p_cbid); });
	return 0;
}

int LuaClass_Globals::LUA_addresourceeventcb(lua_State* p_L)
{
    LuaContext::AddCallback(p_L, [](const std::string& p_cbid, int p_reffunc) { MainService::getInstance()->RegisterResourceEventCallback(p_cbid, p_reffunc); });
    return 0;
}

int LuaClass_Globals::LUA_removeresourceeventcb(lua_State* p_L)
{
    LuaContext::RemoveCallback(p_L, [](const std::string& p_cbid)->int { return MainService::getInstance()->UnregisterResourceEventCallback(p_cbid); });
    return 0;
}

int LuaClass_Globals::LUA_showmousecursor( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Globals::show_mousecursor : argument(s) missing" );
	}

    bool disp = luaL_checkint( p_L, 1 );
    MainService::getInstance()->RequestMouseCursorDisplayState( disp );

    return 0;
}

int LuaClass_Globals::LUA_setmousecursorcircularmode( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Globals::set_mousecursorcircularmode : argument(s) missing" );
	}

    bool disp = luaL_checkint( p_L, 1 );
    MainService::getInstance()->RequestMouseCursorCircularMode( disp );

    return 0;
}

int LuaClass_Globals::LUA_totalmem( lua_State* p_L )
{
    lua_pushinteger( p_L, DrawSpace::Utils::MemAlloc::GetInstance()->GetTotalSize() );
    return 1;
}

int LuaClass_Globals::LUA_log(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("Globals::log : argument(s) missing");
    }
    int level = luaL_checkint(p_L, 1);
    dsstring log = luaL_checkstring(p_L, 2);
    MainService::getInstance()->RequestLog( level, log );

    return 0;
}

int LuaClass_Globals::LUA_formatreal(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("Globals::format_real : argument(s) missing");
    }
    dsreal value = luaL_checknumber(p_L, 1);
    int precision = luaL_checkint(p_L, 2);

    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    dsstring s = stream.str();
    
    lua_pushstring(p_L, stream.str().c_str());
    return 1;
}

int LuaClass_Globals::LUA_breakpoint(lua_State* p_L)
{
	int argc = lua_gettop(p_L);
	if (argc > 0)
	{
		dsstring dbgstr = luaL_checkstring(p_L, 1);
		_asm nop
	}
	return 0;
}

int LuaClass_Globals::LUA_reset( lua_State* p_L )
{
    MainService::getInstance()->RequestLuaStackReset();
    return 0;
}

int LuaClass_Globals::LUA_settexturesrootpath( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Globals::set_texturesrootpath : argument(s) missing" );
	}

	dsstring path = luaL_checkstring( p_L, 1 );

    Texture::SetRootPath( path ); // temporaire, a supprimer quand tout lua stack utilisera le resource system

    DrawSpace::Systems::ResourcesSystem::SetTexturesRootPath(path);

    return 0;
}

int LuaClass_Globals::LUA_setshadersrootpath( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Globals::set_shadersrootpath : argument(s) missing" );
	}

	dsstring path = luaL_checkstring( p_L, 1 );

    Shader::SetRootPath( path ); // temporaire, a supprimer quand tout lua stack utilisera le resource system

    DrawSpace::Systems::ResourcesSystem::SetShadersRootPath(path);

    return 0;
}

int LuaClass_Globals::LUA_enableshadersdescrinfinalpath(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Globals::enable_shadersdescrinfinalpath : argument(s) missing");
    }

    bool enable = luaL_checkint(p_L, 1);
    DrawSpace::Systems::ResourcesSystem::EnableShadersDescrInFinalPath(enable);

    return 0;
}

int LuaClass_Globals::LUA_setmeshesrootpath( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Globals::set_meshesrootpath : argument(s) missing" );
	}

	dsstring path = luaL_checkstring( p_L, 1 );


    DrawSpace::Systems::ResourcesSystem::SetMeshesRootPath(path);

    return 0;
}

int LuaClass_Globals::LUA_setscriptsrootpath( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Globals::set_scriptsrootpath : argument(s) missing" );
	}

	dsstring path = luaL_checkstring( p_L, 1 );

    LuaContext::getInstance()->SetRootPath( path );

    return 0;
}

int LuaClass_Globals::LUA_setvirtualfs( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Globals::set_virtualfs : argument(s) missing" );
	}

	dsstring path = luaL_checkstring( p_L, 1 );

    File::MountVirtualFS( path );
    return 0;
}

int LuaClass_Globals::LUA_sleep( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Globals::sleep : argument(s) missing" );
	}

    int delayMs = luaL_checkint( p_L, 1 );
    Sleep( delayMs );
    return 0;
}

int LuaClass_Globals::LUA_signalrenderscenebegin( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Globals::signal_renderscenebegin : argument(s) missing" );
	}

    dsstring entitygraph_id = luaL_checkstring( p_L, 1 );

    MainService::getInstance()->RequestSignalRenderSceneBegin( entitygraph_id );
    return 0;
}

int LuaClass_Globals::LUA_signalrendersceneend( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Globals::signal_rendersceneend : argument(s) missing" );
	}

    dsstring entitygraph_id = luaL_checkstring( p_L, 1 );

    MainService::getInstance()->RequestSignalRenderSceneEnd( entitygraph_id );
    return 0;
}

int LuaClass_Globals::LUA_dsexception(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Globals::ds_exception : argument(s) missing");
    }

    dsstring text = luaL_checkstring(p_L, 1);
    //_DSEXCEPTION( text );
    LUA_ERROR(text);

    return 0;
}

int LuaClass_Globals::LUA_releaseassets(lua_State* p_L)
{
    MainService::getInstance()->RequestReleaseAssets();
    return 0;
}

int LuaClass_Globals::LUA_round(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Globals::round : argument(s) missing");
    }

    dsreal value = luaL_checknumber(p_L, 1);

    lua_pushinteger(p_L, static_cast<int>(value));
    return 1;
}

int LuaClass_Globals::LUA_pow(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("Globals::pow : argument(s) missing");
    }

    dsreal value = luaL_checknumber(p_L, 1);
    dsreal power = luaL_checknumber(p_L, 2);

    dsreal res = std::pow(value, power);

    lua_pushnumber(p_L, res);
    return 1;
}

int LuaClass_Globals::LUA_clamp(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 3)
    {
        LUA_ERROR("Globals::clamp : argument(s) missing");
    }

    dsreal a = luaL_checknumber(p_L, 1);
    dsreal b = luaL_checknumber(p_L, 2);
    dsreal x = luaL_checknumber(p_L, 3);

    lua_pushnumber(p_L, Maths::Clamp(a, b, x));
    return 1;
}

int LuaClass_Globals::LUA_stoi(lua_State* p_L)
{
	int argc = lua_gettop(p_L);
	if (argc < 1)
	{
		LUA_ERROR("Globals::stoi : argument(s) missing");
	}
	dsstring svalue = luaL_checkstring(p_L, 1);
	int value = 0;
	int error = 0;
	try
	{
		value = std::stoi(svalue);
	}
	catch (const std::invalid_argument&)
	{
		error = 1;
	}
	catch (const std::out_of_range&)
	{
		error = 1;
	}
	lua_pushinteger(p_L, value);
	lua_pushinteger(p_L, error);
	return 2;
}

int LuaClass_Globals::LUA_cos(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Globals::cos : argument(s) missing");
    }

    dsreal a = luaL_checknumber(p_L, 1);
    dsreal cosa = std::cos(a);
    lua_pushnumber(p_L, cosa);
    return 1;
}

int LuaClass_Globals::LUA_sin(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Globals::cos : argument(s) missing");
    }

    dsreal a = luaL_checknumber(p_L, 1);
    dsreal sina = std::sin(a);
    lua_pushnumber(p_L, sina);
    return 1;
}

int LuaClass_Globals::LUA_ctos(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 3)
    {
        LUA_ERROR("Globals::ctos : argument(s) missing");
    }

    dsreal x = luaL_checknumber(p_L, 1);
    dsreal y = luaL_checknumber(p_L, 2);
    dsreal z = luaL_checknumber(p_L, 3);

    Vector c(x, y, z, 1.0);
    Vector s;
    Maths::CartesiantoSpherical(c, s);
       
    dsreal longit{ Maths::RadToDeg(s[1]) };
    dsreal latit{ Maths::RadToDeg(s[2]) };

    lua_pushnumber(p_L, s[0]);
    lua_pushnumber(p_L, longit);
    lua_pushnumber(p_L, latit);
    return 3;
}

int LuaClass_Globals::LUA_stoc(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 3)
    {
        LUA_ERROR("Globals::stoc : argument(s) missing");
    }

    dsreal r = luaL_checknumber(p_L, 1);
    dsreal t = Maths::DegToRad(luaL_checknumber(p_L, 2));
    dsreal p = Maths::DegToRad(luaL_checknumber(p_L, 3));

    Vector s(r, t, p, 1.0);
    Vector c;
    Maths::SphericaltoCartesian(s, c);

    lua_pushnumber(p_L, c[0]);
    lua_pushnumber(p_L, c[1]);
    lua_pushnumber(p_L, c[2]);
    return 3;
}

int LuaClass_Globals::LUA_rotatequaternionx(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 3)
    {
        LUA_ERROR("Globals::rotate_quaternion_x : argument(s) missing");
    }

    dsreal angle_deg{ luaL_checknumber(p_L, 1) };
    dsreal angle_rad{ Maths::DegToRad(angle_deg) };

    LuaClass_Vector*        lua_axis_vector     { Luna<LuaClass_Vector>::check(p_L, 2) };
    LuaClass_Quaternion*    lua_quat            { Luna<LuaClass_Quaternion>::check(p_L, 3) };

    Quaternion q, qres;

    q.RotationAxis(lua_axis_vector->getVector(), angle_rad);
    qres = lua_quat->GetQuaternion() * q; 

    // update input quaternion
    lua_quat->SetQuaternion(qres);

    Utils::Matrix orientation;
    qres.RotationMatFrom(orientation);

    // update input axis vector
    Vector out_axis_vector(orientation(0, 0), orientation(0, 1), orientation(0, 2), 1.0);
    lua_axis_vector->setVector(out_axis_vector);

    return 0;
}

int LuaClass_Globals::LUA_rotatequaterniony(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 3)
    {
        LUA_ERROR("Globals::rotate_quaternion_y : argument(s) missing");
    }

    dsreal angle_deg{ luaL_checknumber(p_L, 1) };
    dsreal angle_rad{ Maths::DegToRad(angle_deg) };

    LuaClass_Vector*        lua_axis_vector{ Luna<LuaClass_Vector>::check(p_L, 2) };
    LuaClass_Quaternion*    lua_quat{ Luna<LuaClass_Quaternion>::check(p_L, 3) };

    Quaternion q, qres;

    q.RotationAxis(lua_axis_vector->getVector(), angle_rad);
    qres = lua_quat->GetQuaternion() * q;

    // update input quaternion
    lua_quat->SetQuaternion(qres);

    Utils::Matrix orientation;
    qres.RotationMatFrom(orientation);

    // update input axis vector
    Vector out_axis_vector(orientation(1, 0), orientation(1, 1), orientation(1, 2), 1.0);
    lua_axis_vector->setVector(out_axis_vector);

    return 0;
}

int LuaClass_Globals::LUA_rotatequaternionz(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 3)
    {
        LUA_ERROR("Globals::rotate_quaternion_z : argument(s) missing");
    }

    dsreal angle_deg{ luaL_checknumber(p_L, 1) };
    dsreal angle_rad{ Maths::DegToRad(angle_deg) };

    LuaClass_Vector* lua_axis_vector{ Luna<LuaClass_Vector>::check(p_L, 2) };
    LuaClass_Quaternion* lua_quat{ Luna<LuaClass_Quaternion>::check(p_L, 3) };

    Quaternion q, qres;

    q.RotationAxis(lua_axis_vector->getVector(), angle_rad);
    qres = lua_quat->GetQuaternion() * q;

    // update input quaternion
    lua_quat->SetQuaternion(qres);

    Utils::Matrix orientation;
    qres.RotationMatFrom(orientation);

    // update input axis vector
    Vector out_axis_vector(orientation(2, 0), orientation(2, 1), orientation(2, 2), 1.0);
    lua_axis_vector->setVector(out_axis_vector);

    return 0;
}

int LuaClass_Globals::LUA_activateresourcessystem(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Globals::activate_resourcessystem : argument(s) missing");
    }

    dsstring contextid = luaL_checkstring(p_L, 1);

    MainService::getInstance()->ActivateResourcesSystem(contextid);
    return 0;
}

int LuaClass_Globals::LUA_deactivateresourcessystem(lua_State* p_L)
{
    MainService::getInstance()->DeactivateResourcesSystem();
    return 0;
}

int LuaClass_Globals::LUA_registerextension(lua_State* p_L)
{
    dsstring extension_name{ luaL_checkstring(p_L, 1) };
    dsstring extension_description;
    const auto extension_instance{ LuaExtLoad::RegisterLuaExtension(extension_name, p_L, extension_description) };
    m_extensions[extension_name] = extension_instance;

    Systems::Hub* hub{ MainService::getInstance()->GetHub() };
    extension_instance->SetHub(hub);

    extension_instance->SetLoggerConfiguration(MainService::getInstance()->GetLogConf());

    lua_pushstring(p_L, extension_description.c_str());
    return 1;
}

int LuaClass_Globals::LUA_dumpallextensionsalloc(lua_State* p_L)
{
    for (auto& e : m_extensions)
    {
        LuaExtension* extension_instance{ e.second };

        dsstring luaext_name = e.first;
        _DSDEBUG(MainService::getInstance()->RequestLogger(), dsstring("BEGIN*******************Dumping ") << luaext_name << dsstring("lua extension mem allocs*******************"));
        extension_instance->GetMemAllocInstance()->DumpContent();
        _DSDEBUG(MainService::getInstance()->RequestLogger(), dsstring("END**************************************************************************************"));
    }
    return 0;
}

int LuaClass_Globals::LUA_getextensionsalloctotalsize(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Globals::get_extensionsalloctotalsize : argument(s) missing");
    }

    dsstring extensionid = luaL_checkstring(p_L, 1);
    int totalsize = 0;
    if (0 == m_extensions.count(extensionid))
    {
        LUA_ERROR("Globals::get_extensionsalloctotalsize : unknown extension id " + extensionid);
    }

    totalsize = m_extensions.at(extensionid)->GetMemAllocInstance()->GetTotalSize();

    lua_pushinteger(p_L, totalsize);
    return 1;
}
