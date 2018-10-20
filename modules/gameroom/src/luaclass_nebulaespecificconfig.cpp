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

using namespace DrawSpace;
using namespace DrawSpace::Utils;

const char LuaClass_NebulaeSpecificConfig::className[] = "NebulaeSpecificConfig";
const Luna<LuaClass_NebulaeSpecificConfig>::RegType LuaClass_NebulaeSpecificConfig::methods[] =
{
    { "apply", &LuaClass_NebulaeSpecificConfig::LUA_apply },
    { 0, 0 }
};

LuaClass_NebulaeSpecificConfig::LuaClass_NebulaeSpecificConfig(lua_State* p_L)
{

    // for test purpose...

    UVPairList uvpl0;
    uvpl0.push_back(std::make_pair<int, int>(1, 2));
    uvpl0.push_back(std::make_pair<int, int>(3, 4));
    dsreal scale0( 1.2 );
    Utils::Vector pos0(10.2, 20.3, 30.4, 1.0);

    m_dataModel.push_back(std::make_tuple(pos0,scale0,uvpl0));


    UVPairList uvpl1;
    uvpl1.push_back(std::make_pair<int, int>(100, 201));
    uvpl1.push_back(std::make_pair<int, int>(34, 44));
    dsreal scale1(0.5);
    Utils::Vector pos1(0.0, 0.3, -1000.0, 1.0);

    m_dataModel.push_back(std::make_tuple(pos1, scale1, uvpl1));
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

    /*
    for(auto& e : m_dataModel )
    {
        dsreal s;
        Utils::Vector p;
        UVPairList    uv;
        std::tie(p, s, uv) = e;

        _asm nop
    }
    */

    entity_rendering_aspect->AddComponent<DataModel>( "nebulae_specific_config", m_dataModel );

    return 0;
}


