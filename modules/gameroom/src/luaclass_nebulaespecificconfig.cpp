/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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
    { "cleanup", &LuaClass_NebulaeSpecificConfig::LUA_cleanup },
    { "create_bloc", &LuaClass_NebulaeSpecificConfig::LUA_createbloc },
    { "set_bloccolor", &LuaClass_NebulaeSpecificConfig::LUA_setbloccolor },
    { "set_blocposition", &LuaClass_NebulaeSpecificConfig::LUA_setblocposition },
    { "set_blocscale", &LuaClass_NebulaeSpecificConfig::LUA_setblocscale },
    { "add_bloctextureuvpair", &LuaClass_NebulaeSpecificConfig::LUA_addbloctextureuvpair },
    { "add_blocmaskuvpair", &LuaClass_NebulaeSpecificConfig::LUA_addblocmaskuvpair },
    { "set_texturesresolutions", &LuaClass_NebulaeSpecificConfig::LUA_settexturesresolutions },
    { 0, 0 }
};

LuaClass_NebulaeSpecificConfig::LuaClass_NebulaeSpecificConfig(lua_State* p_L) :
    m_mask_atlas_resolution( 1 ),
    m_texture_atlas_resolution( 1 )
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

    entity_rendering_aspect->AddComponent<DataModel>( "nebulae_specific_config", m_dataModel );

    std::pair<int,int> texturesAtlasResols = std::make_pair(m_texture_atlas_resolution, m_mask_atlas_resolution);
    entity_rendering_aspect->AddComponent<std::pair<int, int>>("nebulae_texture_atlas_resols", texturesAtlasResols);

    return 0;
}

int LuaClass_NebulaeSpecificConfig::LUA_cleanup(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("NebulaeSpecificConfig::cleanup : argument(s) missing");
    }

    LuaClass_Rendering* lua_rendering = Luna<LuaClass_Rendering>::check(p_L, 1);
    DrawSpace::Aspect::RenderingAspect* entity_rendering_aspect = lua_rendering->GetRenderingAspect();

    entity_rendering_aspect->RemoveComponent<DataModel>("nebulae_specific_config");
    entity_rendering_aspect->RemoveComponent<std::pair<int, int>>("nebulae_texture_atlas_resols");

    return 0;
}

int LuaClass_NebulaeSpecificConfig::LUA_createbloc(lua_State* p_L)
{
    Utils::Vector   default_color(0.0, 0.0, 0.0, 1.0);
    Utils::Vector   default_pos(0.0, 0.0, 0.0, 1.0);
    UVPairList      default_textureuvpair;
    UVPairList      default_maskuvpair;

    m_dataModel.push_back(std::make_tuple(default_color, default_pos, 1.0, default_textureuvpair, default_maskuvpair));

    // return bloc index;
    lua_pushinteger( p_L, m_dataModel.size() - 1);
    return 1;
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

int LuaClass_NebulaeSpecificConfig::LUA_settexturesresolutions(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("Distribution::set_texturesresolutions : argument(s) missing");
    }

    m_texture_atlas_resolution = luaL_checkint(p_L, 1);
    m_mask_atlas_resolution = luaL_checkint(p_L, 2);

    return 0;
}
