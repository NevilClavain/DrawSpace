/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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
#include "AC3DMeshe.h"

#include "luaext_load.h"


using namespace DrawSpace;
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
	MainService::GetInstance()->RequestLuaFileExec(path);

    return 0;
}

int LuaClass_Globals::LUA_dumpmem( lua_State* p_L )
{
    MainService::GetInstance()->RequestMemAllocDump();
    return 0;
}

int LuaClass_Globals::LUA_addappruncb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterRunCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removeappruncb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterRunCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addkeydowncb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterKeyPressCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removekeydowncb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterKeyPressCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addkeyupcb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterEndKeyPressCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removekeyupcb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterEndKeyPressCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addoncharcb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterOnCharCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removeoncharcb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterOnCharCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addmousemovecb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterMouseMoveCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemousemovecb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterMouseMoveCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addmouseleftbuttondowncb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterMouseLeftButtonDownCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemouseleftbuttondowncb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterMouseLeftButtonDownCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addmouseleftbuttonupcb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterMouseLeftButtonUpCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemouseleftbuttonupcb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterMouseLeftButtonUpCallback( p_cbid ); } );
    return 0;
}


int LuaClass_Globals::LUA_addmouserightbuttondowncb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterMouseRightButtonDownCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemouserightbuttondowncb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterMouseRightButtonDownCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addmouserightbuttonupcb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterMouseRightButtonUpCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Globals::LUA_removemouserightbuttonupcb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterMouseRightButtonUpCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Globals::LUA_addanimationeventcb(lua_State* p_L)
{
	LuaContext::AddCallback(p_L, [](const std::string& p_cbid, int p_reffunc) { MainService::GetInstance()->RegisterAnimationEventCallback(p_cbid, p_reffunc); });
	return 0;
}

int LuaClass_Globals::LUA_removeanimationeventcb(lua_State* p_L)
{
	LuaContext::RemoveCallback(p_L, [](const std::string& p_cbid)->int { return MainService::GetInstance()->UnregisterAnimationEventCallback(p_cbid); });
	return 0;
}

int LuaClass_Globals::LUA_addresourceeventcb(lua_State* p_L)
{
    LuaContext::AddCallback(p_L, [](const std::string& p_cbid, int p_reffunc) { MainService::GetInstance()->RegisterResourceEventCallback(p_cbid, p_reffunc); });
    return 0;
}

int LuaClass_Globals::LUA_removeresourceeventcb(lua_State* p_L)
{
    LuaContext::RemoveCallback(p_L, [](const std::string& p_cbid)->int { return MainService::GetInstance()->UnregisterResourceEventCallback(p_cbid); });
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
    MainService::GetInstance()->RequestMouseCursorDisplayState( disp );

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
    MainService::GetInstance()->RequestMouseCursorCircularMode( disp );

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
    MainService::GetInstance()->RequestLog( level, log );

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
    MainService::GetInstance()->RequestLuaStackReset();
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

    AC3DMesheImport::SetRootPath( path ); // temporaire, a supprimer quand tout lua stack utilisera le resource system

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

    LuaContext::GetInstance()->SetRootPath( path );

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

    MainService::GetInstance()->RequestSignalRenderSceneBegin( entitygraph_id );
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

    MainService::GetInstance()->RequestSignalRenderSceneEnd( entitygraph_id );
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
    MainService::GetInstance()->RequestReleaseAssets();
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

int LuaClass_Globals::LUA_activateresourcessystem(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Globals::activate_resourcessystem : argument(s) missing");
    }

    dsstring contextid = luaL_checkstring(p_L, 1);

    MainService::GetInstance()->ActivateResourcesSystem(contextid);
    return 0;
}

int LuaClass_Globals::LUA_deactivateresourcessystem(lua_State* p_L)
{
    MainService::GetInstance()->DeactivateResourcesSystem();
    return 0;
}

int LuaClass_Globals::LUA_registerextension(lua_State* p_L)
{
    dsstring extension_name{ luaL_checkstring(p_L, 1) };
    dsstring extension_description;
    LuaExtension* extension_instance{ LuaExtLoad::RegisterLuaExtension(extension_name, p_L, extension_description) };
    m_extensions[extension_name] = extension_instance;

    lua_pushstring(p_L, extension_description.c_str());
    return 1;
}

int LuaClass_Globals::LUA_dumpallextensionsalloc(lua_State* p_L)
{
    for (auto& e : m_extensions)
    {
        LuaExtension* extension_instance{ e.second };

        dsstring luaext_name = e.first;
        _DSDEBUG(MainService::GetInstance()->RequestLogger(), dsstring("BEGIN*******************Dumping ") << luaext_name << dsstring("lua extension mem allocs*******************"));
        extension_instance->GetMemAllocInstance()->DumpContent();
        _DSDEBUG(MainService::GetInstance()->RequestLogger(), dsstring("END**************************************************************************************"));
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