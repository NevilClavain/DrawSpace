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

#ifndef _LUACLASS_RENDERSTATESSET_H_
#define _LUACLASS_RENDERSTATESSET_H_

#include "luna.h"
#include "renderstatesset.h"

class LuaClass_RenderStatesSet
{
public:
	LuaClass_RenderStatesSet( lua_State* p_L );
	~LuaClass_RenderStatesSet( void );

    int LUA_addrenderstatein( lua_State* p_L );
    int LUA_addrenderstateout( lua_State* p_L );

    int LUA_updaterenderstatein( lua_State* p_L );
    int LUA_updaterenderstateout( lua_State* p_L );

    DrawSpace::Core::RenderStatesSet GetRenderStatesSet( void ) const;

    static const char className[];
    static const Luna<LuaClass_RenderStatesSet>::RegType methods[];

private:
    DrawSpace::Core::RenderStatesSet   m_renderstatesset;
};

#endif
