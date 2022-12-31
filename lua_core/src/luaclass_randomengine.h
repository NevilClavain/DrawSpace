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


#include <random>
#include <memory>
#include "luna.h"

class LuaClass_RandomEngine
{
private:
    std::unique_ptr<std::default_random_engine> m_random_engine;

public:
    LuaClass_RandomEngine(lua_State* p_L);
    ~LuaClass_RandomEngine(void);

    int LUA_setseedfromtime(lua_State* p_L);
    int LUA_setseed(lua_State* p_L);

    std::default_random_engine& GetRandomEngine( void ) const;

    static const char className[];
    static const Luna<LuaClass_RandomEngine>::RegType methods[];
};
