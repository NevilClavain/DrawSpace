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

#pragma once

#include "luna.h"
#include "renderer.h"

class LuaClass_Gui
{
protected:
    DrawSpace::Interface::Renderer* m_renderer;

public:
	LuaClass_Gui( lua_State* p_L );
	~LuaClass_Gui( void );

    int LUA_init( lua_State* p_L );
    int LUA_release( lua_State* p_L );
    int LUA_setresourcespath( lua_State* p_L );
    int LUA_loadscheme( lua_State* p_L );
    int LUA_unloadallschemes( lua_State* p_L );
    int LUA_loadlayout( lua_State* p_L );
    int LUA_unloadalllayouts( lua_State* p_L );
    int LUA_setlayout( lua_State* p_L );
    int LUA_showgui( lua_State* p_L );

    int LUA_setmousecursorimage( lua_State* p_L );
    int LUA_showmousecursor( lua_State* p_L );

    int LUA_onkeydown(lua_State* p_L );
    int LUA_onkeyup(lua_State* p_L );
    int LUA_onchar(lua_State* p_L );

    int LUA_onmousemove(lua_State* p_L );
    int LUA_onmouseleftbuttondown(lua_State* p_L );
    int LUA_onmouseleftbuttonup(lua_State* p_L );
    int LUA_onmouserightbuttondown(lua_State* p_L );
    int LUA_onmouserightbuttonup(lua_State* p_L );

    int LUA_addpushbuttonclickedcb( lua_State* p_L );
    int LUA_removepushbuttonclickedcb( lua_State* p_L );

	int LUA_setwidgettext(lua_State* p_L);
	int LUA_getwidgettext(lua_State* p_L);
    int LUA_setmultilineeditboxcaretindex(lua_State* p_L);


    static const char className[];
    static const Luna<LuaClass_Gui>::RegType methods[];

};

