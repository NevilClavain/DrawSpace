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

#include "luacontext.h"
#include "luaclass_randomengine.h"

const char LuaClass_RandomEngine::className[] = "RandomEngine";
const Luna<LuaClass_RandomEngine>::RegType LuaClass_RandomEngine::methods[] =
{
    { "set_seedfromtime", &LuaClass_RandomEngine::LUA_setseedfromtime },
	{ "set_seed", &LuaClass_RandomEngine::LUA_setseed },
    { 0, 0 }
};

LuaClass_RandomEngine::LuaClass_RandomEngine(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        m_random_engine = std::make_unique<std::default_random_engine>();
    }
    else
    {
        int seed = luaL_checkint(p_L, 1);
        m_random_engine = std::make_unique<std::default_random_engine>(seed);
    }
}

LuaClass_RandomEngine::~LuaClass_RandomEngine(void)
{
}

int LuaClass_RandomEngine::LUA_setseedfromtime(lua_State* p_L)
{
    m_random_engine->seed(::GetTickCount());
    return 0;
}

int LuaClass_RandomEngine::LUA_setseed(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("RandomEngine::set_seed : argument(s) missing");
    }
    int seed_val = luaL_checkint(p_L, 1);
    m_random_engine->seed(seed_val);
    return 0;
}

std::default_random_engine& LuaClass_RandomEngine::GetRandomEngine(void) const
{
    return *m_random_engine;
}
