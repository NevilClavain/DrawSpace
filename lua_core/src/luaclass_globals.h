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

#include <map>
#include "luna.h"

//fwd decl
class LuaExtension;

class LuaClass_Globals
{
private:
    std::map<dsstring, LuaExtension*> m_extensions;

public:
	LuaClass_Globals( lua_State* p_L );
	~LuaClass_Globals( void );

    int LUA_quit( lua_State* p_L );
    int LUA_clearconsole( lua_State* p_L );
    int LUA_print( lua_State* p_L );
    int LUA_dofile( lua_State* p_L );
    int LUA_dumpmem( lua_State* p_L );
    int LUA_totalmem( lua_State* p_L );
    int LUA_log(lua_State* p_L);
    int LUA_formatreal(lua_State* p_L);

	int LUA_breakpoint(lua_State* p_L);
    
    int LUA_addappruncb( lua_State* p_L );
    int LUA_removeappruncb( lua_State* p_L );

    int LUA_addkeydowncb( lua_State* p_L );
    int LUA_removekeydowncb( lua_State* p_L );

    int LUA_addkeyupcb( lua_State* p_L );
    int LUA_removekeyupcb( lua_State* p_L );

    int LUA_addoncharcb( lua_State* p_L );
    int LUA_removeoncharcb( lua_State* p_L );

    int LUA_addmousemovecb( lua_State* p_L );
    int LUA_removemousemovecb( lua_State* p_L );

    int LUA_addmouseleftbuttondowncb( lua_State* p_L );
    int LUA_removemouseleftbuttondowncb( lua_State* p_L );

    int LUA_addmouseleftbuttonupcb( lua_State* p_L );
    int LUA_removemouseleftbuttonupcb( lua_State* p_L );

    int LUA_addmouserightbuttondowncb( lua_State* p_L );
    int LUA_removemouserightbuttondowncb( lua_State* p_L );

    int LUA_addmouserightbuttonupcb( lua_State* p_L );
    int LUA_removemouserightbuttonupcb( lua_State* p_L );

	int LUA_addanimationeventcb(lua_State* p_L);
	int LUA_removeanimationeventcb(lua_State* p_L);

    int LUA_addresourceeventcb(lua_State* p_L);
    int LUA_removeresourceeventcb(lua_State* p_L);

	int LUA_stoi(lua_State* p_L);
    int LUA_clamp(lua_State* p_L);

    int LUA_cos(lua_State* p_L);
    int LUA_sin(lua_State* p_L);

    int LUA_ctos(lua_State* p_L);
    int LUA_stoc(lua_State* p_L);

    int LUA_rotatequaternionx(lua_State* p_L);
    int LUA_rotatequaterniony(lua_State* p_L);
    int LUA_rotatequaternionz(lua_State* p_L);


    int LUA_showmousecursor( lua_State* p_L );
    int LUA_setmousecursorcircularmode( lua_State* p_L );

    int LUA_reset( lua_State* p_L );

    int LUA_sleep( lua_State* p_L );

    int LUA_signalrenderscenebegin( lua_State* p_L );
    int LUA_signalrendersceneend( lua_State* p_L );

    int LUA_settexturesrootpath( lua_State* p_L );
    int LUA_setshadersrootpath( lua_State* p_L );
    int LUA_enableshadersdescrinfinalpath(lua_State* p_L);
    int LUA_setmeshesrootpath( lua_State* p_L );
    int LUA_setscriptsrootpath( lua_State* p_L );
    

    int LUA_setvirtualfs( lua_State* p_L );

    int LUA_dsexception( lua_State* p_L );

    int LUA_releaseassets(lua_State* p_L);

    int LUA_round(lua_State* p_L);
    int LUA_pow(lua_State* p_L);

    int LUA_activateresourcessystem(lua_State* p_L);
    int LUA_deactivateresourcessystem(lua_State* p_L);

    int LUA_registerextension(lua_State* p_L);
    int LUA_dumpallextensionsalloc(lua_State* p_L);
    int LUA_getextensionsalloctotalsize(lua_State* p_L);

    static const char className[];
    static const Luna<LuaClass_Globals>::RegType methods[];
};

