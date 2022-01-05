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
#include "luaclass_renderlayer.h"


const char LuaClass_RenderLayer::className[] = "RenderLayer";
const Luna<LuaClass_RenderLayer>::RegType LuaClass_RenderLayer::methods[] =
{
    { "add_renderconfig", &LuaClass_RenderLayer::LUA_addrenderconfig },
    { 0, 0 }
};

LuaClass_RenderLayer::LuaClass_RenderLayer(lua_State* p_L)
{
}

LuaClass_RenderLayer::~LuaClass_RenderLayer(void)
{
}

int LuaClass_RenderLayer::LUA_addrenderconfig(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("RenderLayer::add_renderconfig : argument(s) missing");
    }
    LuaClass_RenderConfig* lua_rc = Luna<LuaClass_RenderConfig>::check(p_L, 1);
    int layer_index = luaL_checkint(p_L, 2);
    
    m_renderConfigs.push_back(std::make_pair(layer_index, lua_rc->GetData()));

    return 0;
}

int LuaClass_RenderLayer::GetRenderConfigListSize(void) const
{
    return m_renderConfigs.size();
}

std::pair<int, LuaClass_RenderConfig::Data> LuaClass_RenderLayer::GetRenderConfig(int p_index) const
{
    return m_renderConfigs[p_index];
}
