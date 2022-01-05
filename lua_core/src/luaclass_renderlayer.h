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

#pragma once

#include "luna.h"
#include "luaclass_renderconfig.h"

class LuaClass_RenderLayer
{
protected:

    //std::vector<LuaClass_RenderConfig::Data>     m_renderConfigs;
    std::vector<std::pair<int,LuaClass_RenderConfig::Data>>     m_renderConfigs;


public:

    LuaClass_RenderLayer(lua_State* p_L);
    ~LuaClass_RenderLayer(void);

    int LUA_addrenderconfig(lua_State* p_L);

    int GetRenderConfigListSize(void) const;
    std::pair<int,LuaClass_RenderConfig::Data> GetRenderConfig(int p_index) const;

    static const char className[];
    static const Luna<LuaClass_RenderLayer>::RegType methods[];

};
