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
    { "create_bloc", &LuaClass_NebulaeSpecificConfig::LUA_createbloc },
    { "set_bloccolor", &LuaClass_NebulaeSpecificConfig::LUA_setbloccolor },
    { "set_blocposition", &LuaClass_NebulaeSpecificConfig::LUA_setblocposition },
    { "set_blocscale", &LuaClass_NebulaeSpecificConfig::LUA_setblocscale },
    { "add_bloctextureuvpair", &LuaClass_NebulaeSpecificConfig::LUA_addbloctextureuvpair },
    { "add_blocmaskuvpair", &LuaClass_NebulaeSpecificConfig::LUA_addblocmaskuvpair },
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

    /*
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
    */

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

int LuaClass_NebulaeSpecificConfig::LUA_createbloc(lua_State* p_L)
{
    Utils::Vector   default_color(0.0, 0.0, 0.0, 1.0);
    Utils::Vector   default_pos(0.0, 0.0, 0.0, 1.0);
    UVPairList      default_textureuvpair;
    UVPairList      default_maskuvpair;

    m_dataModel.push_back(std::make_tuple(default_color, default_pos, 1.0, default_textureuvpair, default_maskuvpair));

    return 0;
}

int LuaClass_NebulaeSpecificConfig::LUA_setbloccolor(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 4)
    {
        LUA_ERROR("Distribution::set_bloccolor : argument(s) missing");
    }

    size_t bloc_index = luaL_checkint(p_L, 1);
    dsreal r = luaL_checknumber(p_L, 2);
    dsreal g = luaL_checknumber(p_L, 3);
    dsreal b = luaL_checknumber(p_L, 4);

    if(bloc_index >= m_dataModel.size())
    {
        LUA_ERROR("Distribution::set_bloccolor : bad index");
    }

    Utils::Vector& color = std::get<0>( m_dataModel[bloc_index] );
    color[0] = r;
    color[1] = g;
    color[2] = b;

    return 0;
}

int LuaClass_NebulaeSpecificConfig::LUA_setblocposition(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 4)
    {
        LUA_ERROR("Distribution::set_blocposition : argument(s) missing");
    }

    size_t bloc_index = luaL_checkint(p_L, 1);
    dsreal x = luaL_checknumber(p_L, 2);
    dsreal y = luaL_checknumber(p_L, 3);
    dsreal z = luaL_checknumber(p_L, 4);

    if (bloc_index >= m_dataModel.size())
    {
        LUA_ERROR("Distribution::set_blocposition : bad index");
    }

    Utils::Vector& pos = std::get<1>(m_dataModel[bloc_index]);
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
    return 0;
}

int LuaClass_NebulaeSpecificConfig::LUA_setblocscale(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("Distribution::set_blocscale : argument(s) missing");
    }

    size_t bloc_index = luaL_checkint(p_L, 1);
    dsreal scale_input = luaL_checknumber(p_L, 2);

    if (bloc_index >= m_dataModel.size())
    {
        LUA_ERROR("Distribution::set_blocscale : bad index");
    }

    dsreal& scale = std::get<2>(m_dataModel[bloc_index]);
    scale = scale_input;
    return 0;
}

int LuaClass_NebulaeSpecificConfig::LUA_addbloctextureuvpair(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 3)
    {
        LUA_ERROR("Distribution::add_bloctextureuvpair : argument(s) missing");
    }

    size_t bloc_index = luaL_checkint(p_L, 1);
    int u = luaL_checkint(p_L, 2);
    int v = luaL_checkint(p_L, 3);

    if (bloc_index >= m_dataModel.size())
    {
        LUA_ERROR("Distribution::add_bloctextureuvpair : bad index");
    }

    UVPairList& uvpairlist = std::get<3>(m_dataModel[bloc_index]);
    std::pair<int, int> uv( u, v);  
    uvpairlist.push_back(uv);

    return 0;
}

int LuaClass_NebulaeSpecificConfig::LUA_addblocmaskuvpair(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 3)
    {
        LUA_ERROR("Distribution::add_blocmaskuvpair : argument(s) missing");
    }

    size_t bloc_index = luaL_checkint(p_L, 1);
    int u = luaL_checkint(p_L, 2);
    int v = luaL_checkint(p_L, 3);

    if (bloc_index >= m_dataModel.size())
    {
        LUA_ERROR("Distribution::add_blocmaskuvpair : bad index");
    }

    UVPairList& uvpairlist = std::get<4>(m_dataModel[bloc_index]);
    std::pair<int, int> uv(u, v);
    uvpairlist.push_back(uv);

    return 0;
}

