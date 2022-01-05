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

#include "luacontext.h"
#include "luaclass_gui.h"
#include "mainservice.h"
#include "plugin.h"

const char LuaClass_Gui::className[] = "Gui";
const Luna<LuaClass_Gui>::RegType LuaClass_Gui::methods[] =
{
    { "init", &LuaClass_Gui::LUA_init },
    { "release", &LuaClass_Gui::LUA_release },
    { "set_resourcespath", &LuaClass_Gui::LUA_setresourcespath },
    { "load_scheme", &LuaClass_Gui::LUA_loadscheme },
    { "unload_allschemes", &LuaClass_Gui::LUA_unloadallschemes },
    { "load_layout", &LuaClass_Gui::LUA_loadlayout },
    { "unload_alllayouts", &LuaClass_Gui::LUA_unloadalllayouts },
    { "set_layout", &LuaClass_Gui::LUA_setlayout },
    { "show_gui", &LuaClass_Gui::LUA_showgui },

    { "set_mousecursorimage", &LuaClass_Gui::LUA_setmousecursorimage },
    { "show_mousecursor", &LuaClass_Gui::LUA_showmousecursor },

    { "on_keydown", &LuaClass_Gui::LUA_onkeydown },
    { "on_keyup", &LuaClass_Gui::LUA_onkeyup },
    { "on_char", &LuaClass_Gui::LUA_onchar },
    { "on_mousemove", &LuaClass_Gui::LUA_onmousemove },
    { "on_mouseleftbuttondown", &LuaClass_Gui::LUA_onmouseleftbuttondown },
    { "on_mouseleftbuttonup", &LuaClass_Gui::LUA_onmouseleftbuttonup },
    { "on_mouserightbuttondown", &LuaClass_Gui::LUA_onmouserightbuttondown },
    { "on_mouserightbuttonup", &LuaClass_Gui::LUA_onmouserightbuttonup },

    { "add_pushbuttonclickedcb", &LuaClass_Gui::LUA_addpushbuttonclickedcb },
    { "remove_buttonclickedcb", &LuaClass_Gui::LUA_removepushbuttonclickedcb },

	{ "set_widgettext", &LuaClass_Gui::LUA_setwidgettext },
	{ "get_widgettext", &LuaClass_Gui::LUA_getwidgettext },
    { "set_multilineeditboxcaretindex", &LuaClass_Gui::LUA_setmultilineeditboxcaretindex },
    
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

int LuaClass_Gui::LUA_release( lua_State* p_L )
{
    m_renderer->GUI_ReleaseSubSystem();
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

int LuaClass_Gui::LUA_unloadallschemes( lua_State* p_L )
{
    m_renderer->GUI_UnloadAllSchemes();
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

int LuaClass_Gui::LUA_unloadalllayouts( lua_State* p_L )
{
    m_renderer->GUI_UnloadAllLayouts();
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

int LuaClass_Gui::LUA_setmousecursorimage( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Gui::set_mousecursorimage : argument(s) missing" );
	}

    dsstring path = luaL_checkstring( p_L, 1 );
    m_renderer->GUI_SetMouseCursorImage( path );
    return 0;
}

int LuaClass_Gui::LUA_showmousecursor( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Gui::show_mousecursor : argument(s) missing" );
	}

    bool disp = luaL_checkint( p_L, 1 );
    m_renderer->GUI_ShowMouseCursor( disp );

    return 0;
}


int LuaClass_Gui::LUA_onkeydown(lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Gui::on_keydown : argument(s) missing" );
	}
    int key = luaL_checkint( p_L, 1 );
    m_renderer->GUI_OnKeyDown( key );

    return 0;
}

int LuaClass_Gui::LUA_onkeyup(lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Gui::on_keyup : argument(s) missing" );
	}
    int key = luaL_checkint( p_L, 1 );
    m_renderer->GUI_OnKeyUp( key );

    return 0;
}

int LuaClass_Gui::LUA_onchar(lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "Gui::on_char : argument(s) missing" );
	}
    int ch = luaL_checkint( p_L, 1 );
    m_renderer->GUI_OnChar( ch );
    return 0;
}

int LuaClass_Gui::LUA_onmousemove(lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 4 )
	{		
        LUA_ERROR( "Gui::on_mousemove : argument(s) missing" );
	}
    int xm = luaL_checkint( p_L, 1 );
    int ym = luaL_checkint( p_L, 2 );
    int dx = luaL_checkint( p_L, 3 );
    int dy = luaL_checkint( p_L, 4 );

    m_renderer->GUI_OnMouseMove( xm, ym, dx, dy );

    return 0;
}

int LuaClass_Gui::LUA_onmouseleftbuttondown(lua_State* p_L )
{
    m_renderer->GUI_OnMouseLeftButtonDown();

    return 0;
}

int LuaClass_Gui::LUA_onmouseleftbuttonup(lua_State* p_L )
{
    m_renderer->GUI_OnMouseLeftButtonUp();
    return 0;
}

int LuaClass_Gui::LUA_onmouserightbuttondown(lua_State* p_L )
{
    m_renderer->GUI_OnMouseRightButtonDown();
    return 0;
}

int LuaClass_Gui::LUA_onmouserightbuttonup(lua_State* p_L )
{
    m_renderer->GUI_OnMouseRightButtonUp();
    return 0;
}

int LuaClass_Gui::LUA_addpushbuttonclickedcb( lua_State* p_L )
{
    LuaContext::AddCallback( p_L, []( const std::string& p_cbid, int p_reffunc ) { MainService::GetInstance()->RegisterGuiPushButtonClickedCallback( p_cbid, p_reffunc ); } );
    return 0;
}

int LuaClass_Gui::LUA_removepushbuttonclickedcb( lua_State* p_L )
{
    LuaContext::RemoveCallback( p_L, []( const std::string& p_cbid )->int { return MainService::GetInstance()->UnregisterGuiPushButtonClickedCallback( p_cbid ); } );
    return 0;
}

int LuaClass_Gui::LUA_setwidgettext(lua_State* p_L)
{
	int argc = lua_gettop(p_L);
	if (argc < 3)
	{
		LUA_ERROR("Gui::set_widgettext : argument(s) missing");
	}

	dsstring layout_name = luaL_checkstring(p_L, 1);
	dsstring widget_name = luaL_checkstring(p_L, 2);
	dsstring text = luaL_checkstring(p_L, 3);

	m_renderer->GUI_SetWidgetText(layout_name, widget_name, text);

	return 0;
}

int LuaClass_Gui::LUA_getwidgettext(lua_State* p_L)
{
	int argc = lua_gettop(p_L);
	if (argc < 2)
	{
		LUA_ERROR("Gui::get_widgettext : argument(s) missing");
	}

	dsstring layout_name = luaL_checkstring(p_L, 1);
	dsstring widget_name = luaL_checkstring(p_L, 2);

	dsstring text;

	m_renderer->GUI_GetWidgetText(layout_name, widget_name, text);

	lua_pushstring(p_L, text.c_str());
	return 1;
}

int LuaClass_Gui::LUA_setmultilineeditboxcaretindex(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 3)
    {
        LUA_ERROR("Gui::set_multilineeditboxcaretindex : argument(s) missing");
    }

    dsstring layout_name = luaL_checkstring(p_L, 1);
    dsstring widget_name = luaL_checkstring(p_L, 2);
    int index = luaL_checkint(p_L, 3);

    m_renderer->GUI_SetMultiLineEditboxCaretIndex(layout_name, widget_name, index);

    return 0;
}

