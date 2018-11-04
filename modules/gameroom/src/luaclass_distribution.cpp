
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

#include "luaclass_randomengine.h"
#include "luacontext.h"
#include "luaclass_distribution.h"

const char LuaClass_Distribution::className[] = "Distribution";
const Luna<LuaClass_Distribution>::RegType LuaClass_Distribution::methods[] =
{
    { "generate", &LuaClass_Distribution::LUA_generate },
    { 0, 0 }
};

LuaClass_Distribution::LuaClass_Distribution(lua_State* p_L)
{    
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Distribution::Distribution : argument(s) missing");
    }

    dsstring meshe_distribution_type = luaL_checkstring(p_L, 1);

    static std::map<dsstring, std::function<void(void)>> distrib_type_aig =
    {
        { "uniform_int_distribution", [&](void) 
        {  
            if (argc < 3)
            {
                LUA_ERROR("Distribution::Distribution : argument(s) missing");
            }

            int min = luaL_checkint(p_L, 2);
            int max = luaL_checkint(p_L, 3);
            m_distribution = std::make_unique<DistributionWrapperImpl<std::uniform_int_distribution<int>>>(min, max);
        } },
        { "uniform_real_distribution", [&](void)
        {
            if (argc < 3)
            {
                LUA_ERROR("Distribution::Distribution : argument(s) missing");
            }

            dsreal min = luaL_checkint(p_L, 2);
            dsreal max = luaL_checkint(p_L, 3);
            m_distribution = std::make_unique<DistributionWrapperImpl<std::uniform_real_distribution<dsreal>>>(min, max);
        } },
    };

    if(distrib_type_aig.count(meshe_distribution_type))
    {
        distrib_type_aig[meshe_distribution_type]();
        m_distribution_type = meshe_distribution_type;
    }
    else
    {
        LUA_ERROR("Distribution::Distribution : unknown distribution type");
    }
}

LuaClass_Distribution::~LuaClass_Distribution(void)
{
}


int LuaClass_Distribution::LUA_generate(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Distribution::generate : argument(s) missing");
    }
    LuaClass_RandomEngine* lua_random_engine = Luna<LuaClass_RandomEngine>::check(p_L, 1);

    static std::map<dsstring, std::function<void(void)>> distrib_generate_aig =
    {
        { "uniform_int_distribution", [&](void)
        {
            DistributionWrapper* dw = m_distribution.get();
            DistributionWrapperImpl<std::uniform_int_distribution<int>>* distribution = static_cast<DistributionWrapperImpl<std::uniform_int_distribution<int>>*>(dw);

            int val = distribution->Generate<int>(lua_random_engine->GetRandomEngine());

            lua_pushinteger(p_L, val);

        } },
        { "uniform_real_distribution", [&](void)
        {
        } },
    };

    if (distrib_generate_aig.count(m_distribution_type))
    {
        distrib_generate_aig[m_distribution_type]();
    }
    else
    {
        LUA_ERROR("Distribution::Distribution : unknown distribution type");
    }
    return 1;
}