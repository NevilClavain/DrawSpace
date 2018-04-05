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
#include "luaclass_gui.h"
#include "mainservice.h"
#include "plugin.h"

const char LuaClass_Gui::className[] = "Gui";
const Luna<LuaClass_Gui>::RegType LuaClass_Gui::methods[] =
{
    { "init", &LuaClass_Gui::LUA_init },
    { "set_resourcespath", &LuaClass_Gui::LUA_setresourcespath },
    { "load_scheme", &LuaClass_Gui::LUA_loadscheme },
    { "load_layout", &LuaClass_Gui::LUA_loadlayout },
    { "set_layout", &LuaClass_Gui::LUA_setlayout },
    { "show_gui", &LuaClass_Gui::LUA_showgui },
	{ 0, 0 }
};

LuaClass_Gui::LuaClass_Gui( lua_State* p_L )
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
}

LuaClass_Gui::~LuaClass_Gui( void )
{
}

int LuaClass_Gui::LUA_init( lua_State* p_L )
{
    m_renderer->GUI_InitSubSystem();
    return 0;
}

int LuaClass_Gui::LUA_setresourcespath( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Gui::set_resourcespath : argument(s) missing" );
	}

    dsstring path = luaL_checkstring( p_L, 1 );
    m_renderer->GUI_SetResourcesRootDirectory( path );

    return 0;
}

int LuaClass_Gui::LUA_loadscheme( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Gui::load_scheme : argument(s) missing" );
	}

    dsstring path = luaL_checkstring( p_L, 1 );
    m_renderer->GUI_LoadScheme( path );

    return 0;
}

int LuaClass_Gui::LUA_loadlayout( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{		
        LUA_ERROR( "Gui::load_layout : argument(s) missing" );
	}

    dsstring path = luaL_checkstring( p_L, 1 );
    dsstring widgets = luaL_checkstring( p_L, 2 );
    m_renderer->GUI_LoadLayout( path, widgets );

    return 0;
}

int LuaClass_Gui::LUA_setlayout( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Gui::set_layout : argument(s) missing" );
	}

    dsstring path = luaL_checkstring( p_L, 1 );
    m_renderer->GUI_SetLayout( path );

    return 0;
}

int LuaClass_Gui::LUA_showgui( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Gui::show_gui : argument(s) missing" );
	}

    bool disp = luaL_checkint( p_L, 1 );

    MainService::GetInstance()->RequestGuiDisplay( disp );

    return 0;
}