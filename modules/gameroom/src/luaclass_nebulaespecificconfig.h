#pragma once
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

#ifndef _LUACLASS_NEBULAESPECIFICCONFIG_H_
#define _LUACLASS_NEBULAESPECIFICCONFIG_H_

#include "luna.h"

class LuaClass_NebulaeSpecificConfig
{
private:

public:
    LuaClass_NebulaeSpecificConfig(lua_State* p_L);
    ~LuaClass_NebulaeSpecificConfig(void);

    int LUA_apply(lua_State* p_L);
   
    static const char className[];
    static const Luna<LuaClass_NebulaeSpecificConfig>::RegType methods[];
};
#endif

