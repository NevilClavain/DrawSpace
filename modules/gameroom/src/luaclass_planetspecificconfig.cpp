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
#include "luaclass_planetspecificconfig.h"
#include "renderingaspect.h"
#include "luaclass_rendering.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;

const char LuaClass_PlanetSpecificConfig::className[] = "PlanetSpecificConfig";
const Luna<LuaClass_PlanetSpecificConfig>::RegType LuaClass_PlanetSpecificConfig::methods[] =
{
    { "apply", &LuaClass_PlanetSpecificConfig::LUA_apply },
    { "cleanup", &LuaClass_PlanetSpecificConfig::LUA_cleanup },
    { "set_planetray", &LuaClass_PlanetSpecificConfig::LUA_setplanetray },
    { "set_amplitudes", &LuaClass_PlanetSpecificConfig::LUA_setamplitudes },
    { "set_offsets", &LuaClass_PlanetSpecificConfig::LUA_setoffsets },
    { "set_seeds", &LuaClass_PlanetSpecificConfig::LUA_setseeds },
    { "set_splattingparams", &LuaClass_PlanetSpecificConfig::LUA_setsplattingparams },
    { "set_fogandatmoparams", &LuaClass_PlanetSpecificConfig::LUA_setfogandatmoparams },
    { "set_terrainbumpfactor", &LuaClass_PlanetSpecificConfig::LUA_setterrainbumpfactor },
    { "enable_landplacepatch", &LuaClass_PlanetSpecificConfig::LUA_enablelandplacepatch },
    { 0, 0 }
};

LuaClass_PlanetSpecificConfig::LuaClass_PlanetSpecificConfig(lua_State* p_L) 
{
}

LuaClass_PlanetSpecificConfig::~LuaClass_PlanetSpecificConfig(void)
{
}


int LuaClass_PlanetSpecificConfig::LUA_apply(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetSpecificConfig::apply : argument(s) missing");
    }

    LuaClass_Rendering* lua_rendering = Luna<LuaClass_Rendering>::check(p_L, 1);
    DrawSpace::Aspect::RenderingAspect* entity_rendering_aspect = lua_rendering->GetRenderingAspect();

    entity_rendering_aspect->AddComponent<dsreal>("planet_ray", m_planets_details.planet_ray);
    entity_rendering_aspect->AddComponent<dsreal>("plains_amplitude", m_planets_details.plains_amplitude);
    entity_rendering_aspect->AddComponent<dsreal>("mountains_amplitude", m_planets_details.mountains_amplitude);
    entity_rendering_aspect->AddComponent<dsreal>("vertical_offset", m_planets_details.vertical_offset);
    entity_rendering_aspect->AddComponent<dsreal>("mountains_offset", m_planets_details.mountains_offset);
    entity_rendering_aspect->AddComponent<dsreal>("plains_seed1", m_planets_details.plains_seed1);
    entity_rendering_aspect->AddComponent<dsreal>("plains_seed2", m_planets_details.plains_seed2);
    entity_rendering_aspect->AddComponent<dsreal>("mix_seed1", m_planets_details.mix_seed1);
    entity_rendering_aspect->AddComponent<dsreal>("mix_seed2", m_planets_details.mix_seed2);

    entity_rendering_aspect->AddComponent<dsreal>("terrainbump_factor", m_planets_details.terrainbump_factor);
    entity_rendering_aspect->AddComponent<dsreal>("splat_transition_up_relative_alt", m_planets_details.splat_transition_up_relative_alt);
    entity_rendering_aspect->AddComponent<dsreal>("splat_transition_down_relative_alt", m_planets_details.splat_transition_down_relative_alt);

    entity_rendering_aspect->AddComponent<int>("splat_texture_resol", m_planets_details.splat_texture_resol);

    entity_rendering_aspect->AddComponent<dsreal>("atmo_kr", m_planets_details.atmo_kr);
    entity_rendering_aspect->AddComponent<dsreal>("fog_alt_limit", m_planets_details.fog_alt_limit);
    entity_rendering_aspect->AddComponent<dsreal>("fog_density", m_planets_details.fog_density);
    entity_rendering_aspect->AddComponent<bool>("enable_landplace_patch", m_planets_details.enable_landplace_patch);

    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_cleanup(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetSpecificConfig::cleanup : argument(s) missing");
    }

    LuaClass_Rendering* lua_rendering = Luna<LuaClass_Rendering>::check(p_L, 1);
    DrawSpace::Aspect::RenderingAspect* entity_rendering_aspect = lua_rendering->GetRenderingAspect();

    entity_rendering_aspect->RemoveComponent<dsreal>("planet_ray");
    entity_rendering_aspect->RemoveComponent<dsreal>("plains_amplitude");
    entity_rendering_aspect->RemoveComponent<dsreal>("mountains_amplitude");
    entity_rendering_aspect->RemoveComponent<dsreal>("vertical_offset");
    entity_rendering_aspect->RemoveComponent<dsreal>("mountains_offset");
    entity_rendering_aspect->RemoveComponent<dsreal>("plains_seed1");
    entity_rendering_aspect->RemoveComponent<dsreal>("plains_seed2");
    entity_rendering_aspect->RemoveComponent<dsreal>("mix_seed1");
    entity_rendering_aspect->RemoveComponent<dsreal>("mix_seed2");
    entity_rendering_aspect->RemoveComponent<dsreal>("terrainbump_factor");
    entity_rendering_aspect->RemoveComponent<dsreal>("splat_transition_up_relative_alt");
    entity_rendering_aspect->RemoveComponent<dsreal>("splat_transition_down_relative_alt");
    entity_rendering_aspect->RemoveComponent<int>("splat_texture_resol");
    entity_rendering_aspect->RemoveComponent<dsreal>("atmo_kr");
    entity_rendering_aspect->RemoveComponent<dsreal>("fog_alt_limit");
    entity_rendering_aspect->RemoveComponent<dsreal>("fog_density");
    entity_rendering_aspect->RemoveComponent<bool>("enable_landplace_patch");

    return 0;
}


int LuaClass_PlanetSpecificConfig::LUA_setplanetray(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetSpecificConfig::set_planetray : argument(s) missing");
    }

    m_planets_details.planet_ray = luaL_checknumber(p_L, 1);
    return 0;
}
int LuaClass_PlanetSpecificConfig::LUA_setamplitudes(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("PlanetSpecificConfig::set_amplitudes : argument(s) missing");
    }

    m_planets_details.plains_amplitude = luaL_checknumber(p_L, 1);
    m_planets_details.mountains_amplitude = luaL_checknumber(p_L, 2);

    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_setoffsets(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("PlanetSpecificConfig::set_offsets : argument(s) missing");
    }

    m_planets_details.vertical_offset = luaL_checknumber(p_L, 1);
    m_planets_details.mountains_offset = luaL_checknumber(p_L, 2);

    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_setseeds(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 4)
    {
        LUA_ERROR("PlanetSpecificConfig::set_seeds : argument(s) missing");
    }

    m_planets_details.plains_seed1 = luaL_checknumber(p_L, 1);
    m_planets_details.plains_seed2 = luaL_checknumber(p_L, 2);
    m_planets_details.mix_seed1 = luaL_checknumber(p_L, 3);
    m_planets_details.mix_seed2 = luaL_checknumber(p_L, 4);

    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_setsplattingparams(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 3)
    {
        LUA_ERROR("PlanetSpecificConfig::set_splattingparams : argument(s) missing");
    }

    m_planets_details.splat_transition_up_relative_alt = luaL_checknumber(p_L, 1);
    m_planets_details.splat_transition_down_relative_alt = luaL_checknumber(p_L, 2);
    m_planets_details.splat_texture_resol = luaL_checknumber(p_L, 3);

    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_setfogandatmoparams(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 3)
    {
        LUA_ERROR("PlanetSpecificConfig::set_fogandatmoparams : argument(s) missing");
    }

    m_planets_details.atmo_kr = luaL_checknumber(p_L, 1);
    m_planets_details.fog_alt_limit = luaL_checknumber(p_L, 2);
    m_planets_details.fog_density = luaL_checknumber(p_L, 3);

    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_setterrainbumpfactor(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetSpecificConfig::set_terrainbumpfactor : argument(s) missing");
    }

    m_planets_details.terrainbump_factor = luaL_checknumber(p_L, 1);
    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_enablelandplacepatch(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetSpecificConfig::enable_landplacepatch : argument(s) missing");
    }

    m_planets_details.enable_landplace_patch = luaL_checkint(p_L, 1);
    return 0;
}
