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

#include "luacontext.h"
#include "luaclass_nebulaespecificconfig.h"
#include "renderingaspect.h"
#include "luaclass_rendering.h"

const char LuaClass_NebulaeSpecificConfig::className[] = "NebulaeSpecificConfig";
const Luna<LuaClass_NebulaeSpecificConfig>::RegType LuaClass_NebulaeSpecificConfig::methods[] =
{
    { "apply", &LuaClass_NebulaeSpecificConfig::LUA_apply },
    { 0, 0 }
};

LuaClass_NebulaeSpecificConfig::LuaClass_NebulaeSpecificConfig(lua_State* p_L)
{
}

LuaClass_NebulaeSpecificConfig::~LuaClass_NebulaeSpecificConfig(void)
{

}


int LuaClass_NebulaeSpecificConfig::LUA_apply(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("NebulaeSpecificConfig::apply : argument(s) missing");
    }

    LuaClass_Rendering* lua_rendering = Luna<LuaClass_Rendering>::check(p_L, 1);
    DrawSpace::Aspect::RenderingAspect* entity_rendering_aspect = lua_rendering->GetRenderingAspect();

    _asm nop
}

