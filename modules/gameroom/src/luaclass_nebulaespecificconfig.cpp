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
    /*
    UVPairList uvpl0;
    uvpl0.push_back(std::make_pair<int, int>(1, 2));
    uvpl0.push_back(std::make_pair<int, int>(3, 4));

    UVPairList maskuvpl0;
    maskuvpl0.push_back(std::make_pair<int, int>(10, 20));
    maskuvpl0.push_back(std::make_pair<int, int>(-30, -40));


    dsreal scale0( 1.2 );
    Utils::Vector pos0(10.2, 20.3, 30.4, 1.0);
    Utils::Vector color0(0.3, 0.4, 0.5, 1.0);

    m_dataModel.push_back(std::make_tuple(color0, pos0,scale0,uvpl0,maskuvpl0));


    UVPairList uvpl1;
    uvpl1.push_back(std::make_pair<int, int>(100, 201));
    uvpl1.push_back(std::make_pair<int, int>(34, 44));

    UVPairList maskuvpl1;
    maskuvpl1.push_back(std::make_pair<int, int>(12, 34));
    maskuvpl1.push_back(std::make_pair<int, int>(56, 78));


    dsreal scale1(0.5);
    Utils::Vector pos1(0.0, 0.3, -1000.0, 1.0);
    Utils::Vector color1(0.3, 0.2, 0.1, 1.0);


    m_dataModel.push_back(std::make_tuple(color1,pos1,scale1,uvpl1,maskuvpl1));
    */


    UVPairList uvpl0;
    uvpl0.push_back(std::make_pair<int, int>(0, 0));
    uvpl0.push_back(std::make_pair<int, int>(0, 1));
    uvpl0.push_back(std::make_pair<int, int>(3, 2));
    uvpl0.push_back(std::make_pair<int, int>(3, 3));
    uvpl0.push_back(std::make_pair<int, int>(5, 6));
    uvpl0.push_back(std::make_pair<int, int>(7, 1));
    uvpl0.push_back(std::make_pair<int, int>(3, 0));
    uvpl0.push_back(std::make_pair<int, int>(3, 4));
    uvpl0.push_back(std::make_pair<int, int>(2, 2));
    uvpl0.push_back(std::make_pair<int, int>(2, 6));
    uvpl0.push_back(std::make_pair<int, int>(2, 1));

    uvpl0.push_back(std::make_pair<int, int>(0, 0));
    uvpl0.push_back(std::make_pair<int, int>(0, 5));
    uvpl0.push_back(std::make_pair<int, int>(3, 5));
    uvpl0.push_back(std::make_pair<int, int>(3, 6));
    uvpl0.push_back(std::make_pair<int, int>(5, 6));
    uvpl0.push_back(std::make_pair<int, int>(7, 1));
    uvpl0.push_back(std::make_pair<int, int>(3, 7));
    uvpl0.push_back(std::make_pair<int, int>(3, 4));
    uvpl0.push_back(std::make_pair<int, int>(2, 1));
    uvpl0.push_back(std::make_pair<int, int>(2, 0));
    uvpl0.push_back(std::make_pair<int, int>(2, 1));

    uvpl0.push_back(std::make_pair<int, int>(0, 0));
    uvpl0.push_back(std::make_pair<int, int>(0, 2));
    uvpl0.push_back(std::make_pair<int, int>(3, 5));
    uvpl0.push_back(std::make_pair<int, int>(3, 5));
    uvpl0.push_back(std::make_pair<int, int>(4, 4));
    uvpl0.push_back(std::make_pair<int, int>(7, 5));
    uvpl0.push_back(std::make_pair<int, int>(3, 1));
    uvpl0.push_back(std::make_pair<int, int>(4, 3));
    uvpl0.push_back(std::make_pair<int, int>(2, 5));
    uvpl0.push_back(std::make_pair<int, int>(2, 7));
    uvpl0.push_back(std::make_pair<int, int>(2, 3));

    uvpl0.push_back(std::make_pair<int, int>(0, 5));
    uvpl0.push_back(std::make_pair<int, int>(0, 1));
    uvpl0.push_back(std::make_pair<int, int>(4, 2));
    uvpl0.push_back(std::make_pair<int, int>(3, 4));
    uvpl0.push_back(std::make_pair<int, int>(4, 6));
    uvpl0.push_back(std::make_pair<int, int>(7, 5));
    uvpl0.push_back(std::make_pair<int, int>(3, 0));
    uvpl0.push_back(std::make_pair<int, int>(4, 1));
    uvpl0.push_back(std::make_pair<int, int>(2, 1));
    uvpl0.push_back(std::make_pair<int, int>(2, 2));
    uvpl0.push_back(std::make_pair<int, int>(2, 4));


    UVPairList maskuvpl0;
    maskuvpl0.push_back(std::make_pair<int, int>(0, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(0, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(3, 2));
    maskuvpl0.push_back(std::make_pair<int, int>(2, 2));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(3, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(0, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 2));
    maskuvpl0.push_back(std::make_pair<int, int>(2, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(3, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(0, 0));
    maskuvpl0.push_back(std::make_pair<int, int>(0, 1));

    maskuvpl0.push_back(std::make_pair<int, int>(1, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(2, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(3, 2));
    maskuvpl0.push_back(std::make_pair<int, int>(2, 2));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(3, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(2, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(0, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(0, 2));
    maskuvpl0.push_back(std::make_pair<int, int>(0, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(0, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(0, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(0, 0));
    maskuvpl0.push_back(std::make_pair<int, int>(0, 1));


    maskuvpl0.push_back(std::make_pair<int, int>(1, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 2));
    maskuvpl0.push_back(std::make_pair<int, int>(2, 2));
    maskuvpl0.push_back(std::make_pair<int, int>(3, 0));
    maskuvpl0.push_back(std::make_pair<int, int>(3, 0));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 0));
    maskuvpl0.push_back(std::make_pair<int, int>(4, 0));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 0));
    maskuvpl0.push_back(std::make_pair<int, int>(3, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(3, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(2, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(2, 0));
    maskuvpl0.push_back(std::make_pair<int, int>(2, 1));


    maskuvpl0.push_back(std::make_pair<int, int>(1, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 2));
    maskuvpl0.push_back(std::make_pair<int, int>(2, 2));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(2, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 3));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 1));
    maskuvpl0.push_back(std::make_pair<int, int>(1, 0));
    maskuvpl0.push_back(std::make_pair<int, int>(3, 0));
    maskuvpl0.push_back(std::make_pair<int, int>(3, 0));
    maskuvpl0.push_back(std::make_pair<int, int>(0, 0));
    maskuvpl0.push_back(std::make_pair<int, int>(0, 1));


    dsreal scale0(1.2);
    Utils::Vector pos0(0.0, 0.0, -1.0, 1.0);
    Utils::Vector color0(1.0, 0.5, 0.5, 1.0);

    m_dataModel.push_back(std::make_tuple(color0, pos0, scale0, uvpl0, maskuvpl0));

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

    entity_rendering_aspect->AddComponent<DataModel>( "nebulae_specific_config", m_dataModel );

    return 0;
}


