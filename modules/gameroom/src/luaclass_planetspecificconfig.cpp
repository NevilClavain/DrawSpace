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
    { "updated", &LuaClass_PlanetSpecificConfig::LUA_updated },
    { "set_resourcespath", &LuaClass_PlanetSpecificConfig::LUA_setresourcespath },
    { "set_planetray", &LuaClass_PlanetSpecificConfig::LUA_setplanetray },
    { "set_atmothickness", &LuaClass_PlanetSpecificConfig::LUA_setatmothickness },
    { "set_amplitudes", &LuaClass_PlanetSpecificConfig::LUA_setamplitudes },
    { "set_offsets", &LuaClass_PlanetSpecificConfig::LUA_setoffsets },
    { "set_seeds", &LuaClass_PlanetSpecificConfig::LUA_setseeds },
    { "set_splattingparams", &LuaClass_PlanetSpecificConfig::LUA_setsplattingparams },
    { "set_fogandatmoparams", &LuaClass_PlanetSpecificConfig::LUA_setfogandatmoparams },
    { "set_terrainbumpfactor", &LuaClass_PlanetSpecificConfig::LUA_setterrainbumpfactor },
    { "set_beachlimit", &LuaClass_PlanetSpecificConfig::LUA_setbeachlimit },
    { "set_climateshaders", &LuaClass_PlanetSpecificConfig::LUA_setclimateshaders },
    { "enable_landplacepatch", &LuaClass_PlanetSpecificConfig::LUA_enablelandplacepatch },
    { "enable_atmosphere", &LuaClass_PlanetSpecificConfig::LUA_enableatmosphere },
    { "enable_light", &LuaClass_PlanetSpecificConfig::LUA_enablelight },
    { "set_lightcolor", &LuaClass_PlanetSpecificConfig::LUA_setlightcolor },
    { "set_lightdir", &LuaClass_PlanetSpecificConfig::LUA_setlightdir },

    { "get_outparam", &LuaClass_PlanetSpecificConfig::LUA_getoutparam },
    { 0, 0 }
};

LuaClass_PlanetSpecificConfig::LuaClass_PlanetSpecificConfig(lua_State* p_L) :
m_rendering_aspect( NULL )
{
    m_planets_details.enable_landplace_patch = false;
    m_planets_details.enable_atmosphere = false;

    // prepare lights tuple
    for( int i = 0; i < 4; i++ )
    {
        std::array<dsreal, 3> light_color = { 1.0, 1.0, 1.0 };
        std::array <dsreal, 3> light_dir = { 0.0, 1.0, 0.0 };
        m_planets_details.lights[i] = std::make_tuple( false, light_color, light_dir);
    }
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

    entity_rendering_aspect->AddComponent<dsstring>("resources_path", m_planets_details.resources_path);
    entity_rendering_aspect->AddComponent<dsreal>("planet_ray", m_planets_details.planet_ray);
    entity_rendering_aspect->AddComponent<dsreal>("atmo_thickness", m_planets_details.atmo_thickness);
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
    entity_rendering_aspect->AddComponent<dsreal>("beach_limit", m_planets_details.beach_limit);
    entity_rendering_aspect->AddComponent<bool>("enable_landplace_patch", m_planets_details.enable_landplace_patch);
    entity_rendering_aspect->AddComponent<bool>("enable_atmosphere", m_planets_details.enable_atmosphere);

    std::pair<dsstring, dsstring> climate_shaders(m_planets_details.climate_vshader, m_planets_details.climate_pshader);
    entity_rendering_aspect->AddComponent<std::pair<dsstring,dsstring>>("climate_shaders", climate_shaders);

    std::vector<PlanetDetails::Lights> lights;
    for( int i = 0; i < 4; i++ )
    {
        lights.push_back(m_planets_details.lights[i] );
    }
    entity_rendering_aspect->AddComponent<std::vector<PlanetDetails::Lights>>("lights", lights);

    ///////////////////////
    // OUT params

    entity_rendering_aspect->AddComponent<int>("OUT_test", 667);


    m_rendering_aspect = entity_rendering_aspect;

    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_cleanup(lua_State* p_L)
{
    if (NULL == m_rendering_aspect)
    {
        _DSEXCEPTION("rendering aspect not set !!!");
    }

    m_rendering_aspect->RemoveComponent<dsstring>("resources_path");
    m_rendering_aspect->RemoveComponent<dsreal>("planet_ray");
    m_rendering_aspect->RemoveComponent<dsreal>("atmo_thickness");
    m_rendering_aspect->RemoveComponent<dsreal>("plains_amplitude");
    m_rendering_aspect->RemoveComponent<dsreal>("mountains_amplitude");
    m_rendering_aspect->RemoveComponent<dsreal>("vertical_offset");
    m_rendering_aspect->RemoveComponent<dsreal>("mountains_offset");
    m_rendering_aspect->RemoveComponent<dsreal>("plains_seed1");
    m_rendering_aspect->RemoveComponent<dsreal>("plains_seed2");
    m_rendering_aspect->RemoveComponent<dsreal>("mix_seed1");
    m_rendering_aspect->RemoveComponent<dsreal>("mix_seed2");
    m_rendering_aspect->RemoveComponent<dsreal>("terrainbump_factor");
    m_rendering_aspect->RemoveComponent<dsreal>("splat_transition_up_relative_alt");
    m_rendering_aspect->RemoveComponent<dsreal>("splat_transition_down_relative_alt");
    m_rendering_aspect->RemoveComponent<int>("splat_texture_resol");
    m_rendering_aspect->RemoveComponent<dsreal>("atmo_kr");
    m_rendering_aspect->RemoveComponent<dsreal>("fog_alt_limit");
    m_rendering_aspect->RemoveComponent<dsreal>("fog_density");
    m_rendering_aspect->RemoveComponent<dsreal>("beach_limit");
    m_rendering_aspect->RemoveComponent<bool>("enable_landplace_patch");
    m_rendering_aspect->RemoveComponent<bool>("enable_atmosphere");
    m_rendering_aspect->RemoveComponent<std::pair<dsstring, dsstring>>("climate_shaders");
    m_rendering_aspect->RemoveComponent<std::vector<PlanetDetails::Lights>>("lights");

    m_rendering_aspect->RemoveComponent<int>("OUT_test");

    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_updated(lua_State* p_L)
{
    if( NULL == m_rendering_aspect )
    {
        _DSEXCEPTION( "rendering aspect not set !!!");
    }

    std::vector<PlanetDetails::Lights> lights;
    for (int i = 0; i < 4; i++)
    {
        lights.push_back(m_planets_details.lights[i]);
    }
    m_rendering_aspect->GetComponent<std::vector<PlanetDetails::Lights>>("lights")->getPurpose() = lights;


    m_rendering_aspect->GetComponent<bool>("enable_atmosphere")->getPurpose() = m_planets_details.enable_atmosphere;

    // signaler le chgt d'un ou plusieurs components...
    m_rendering_aspect->ComponentsUpdated();
    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_setresourcespath(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetSpecificConfig::set_resourcespath : argument(s) missing");
    }

    m_planets_details.resources_path = luaL_checkstring(p_L, 1);

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

int LuaClass_PlanetSpecificConfig::LUA_setatmothickness(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetSpecificConfig::set_atmothickness : argument(s) missing");
    }

    m_planets_details.atmo_thickness = luaL_checknumber(p_L, 1);
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

int LuaClass_PlanetSpecificConfig::LUA_setbeachlimit(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetSpecificConfig::set_beachlimit : argument(s) missing");
    }

    m_planets_details.beach_limit = luaL_checknumber(p_L, 1);
    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_enableatmosphere(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetSpecificConfig::enable_atmosphere : argument(s) missing");
    }

    m_planets_details.enable_atmosphere = luaL_checkint(p_L, 1);
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

int LuaClass_PlanetSpecificConfig::LUA_setclimateshaders(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("PlanetSpecificConfig::set_climateshaders : argument(s) missing");
    }

    m_planets_details.climate_vshader = luaL_checkstring(p_L, 1);
    m_planets_details.climate_pshader = luaL_checkstring(p_L, 2);

    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_enablelight(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("PlanetSpecificConfig::enable_light : argument(s) missing");
    }

    int light_index = luaL_checkint(p_L, 1);
    bool light_state = luaL_checkint(p_L, 2);

    std::get<0>(m_planets_details.lights[light_index]) = light_state;
    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_setlightcolor(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 4)
    {
        LUA_ERROR("PlanetSpecificConfig::set_lightcolor : argument(s) missing");
    }

    int light_index = luaL_checkint(p_L, 1);
    dsreal r = luaL_checknumber(p_L, 2);
    dsreal g = luaL_checknumber(p_L, 3);
    dsreal b = luaL_checknumber(p_L, 4);

    std::array<dsreal, 3> color;
    color[0] = r;
    color[1] = g;
    color[2] = b;
    std::get<1>(m_planets_details.lights[light_index]) = color;
    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_setlightdir(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 4)
    {
        LUA_ERROR("PlanetSpecificConfig::set_lightdir : argument(s) missing");
    }

    int light_index = luaL_checkint(p_L, 1);
    dsreal x = luaL_checknumber(p_L, 2);
    dsreal y = luaL_checknumber(p_L, 3);
    dsreal z = luaL_checknumber(p_L, 4);

    std::array<dsreal, 3> dir;
    dir[0] = x;
    dir[1] = y;
    dir[2] = z;
    std::get<2>(m_planets_details.lights[light_index]) = dir;
    return 0;
}

int LuaClass_PlanetSpecificConfig::LUA_getoutparam(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetSpecificConfig::get_outparam : argument(s) missing");
    }

    dsstring id = luaL_checkstring(p_L, 1);

    static const std::map<dsstring, std::function<void(lua_State* p_L, DrawSpace::Aspect::RenderingAspect* p_rendering_aspect)>> retrieve_param =
    {
        { "OUT_test", [](lua_State* p_L, DrawSpace::Aspect::RenderingAspect* p_rendering_aspect) { lua_pushinteger(p_L, p_rendering_aspect->GetComponent<int>("OUT_test")->getPurpose()); } },
    };

    if(retrieve_param.count(id))
    {
        retrieve_param.at(id)(p_L, m_rendering_aspect);
        return 1;
    }

    LUA_ERROR("PlanetSpecificConfig::get_outparam : Unknown param id : " << id);
    return 0;
}