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

#ifndef _LUACLASS_GLOBALS_H_
#define _LUACLASS_GLOBALS_H_

#include "luna.h"

class LuaClass_Globals
{
protected:

    void add_callback( lua_State* p_L, const std::function<void(const std::string&, int)>& p_register_func );
    void remove_callback( lua_State* p_L, const std::function<int(const std::string&)>& p_unregister_func );

public:
	LuaClass_Globals( lua_State* p_L );
	~LuaClass_Globals( void );

    int LUA_quit( lua_State* p_L );
    int LUA_clearconsole( lua_State* p_L );
    int LUA_print( lua_State* p_L );
    int LUA_dofile( lua_State* p_L );
    int LUA_dumpmem( lua_State* p_L );
    int LUA_totalmem( lua_State* p_L );
    
    int LUA_addappruncb( lua_State* p_L );
    int LUA_removeappruncb( lua_State* p_L );

    int LUA_addkeydowncb( lua_State* p_L );
    int LUA_removekeydowncb( lua_State* p_L );

    int LUA_addkeyupcb( lua_State* p_L );
    int LUA_removekeyupcb( lua_State* p_L );


    int LUA_reset( lua_State* p_L );

    static const char className[];
    static const Luna<LuaClass_Globals>::RegType methods[];
};

#endif