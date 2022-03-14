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
#include "luaclass_planetconfig.h"
#include "renderingaspect.h"
#include "luaclass_planetrendering.h"

#include "luaclass_renderpassnodegraph.h"
#include "texture.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

const char LuaClass_PlanetConfig::className[] = "PlanetConfig";
const Luna<LuaClass_PlanetConfig>::RegType LuaClass_PlanetConfig::methods[] =
{
    { "apply", &LuaClass_PlanetConfig::LUA_apply },
    { "cleanup", &LuaClass_PlanetConfig::LUA_cleanup },
    { "updated", &LuaClass_PlanetConfig::LUA_updated },
    { "set_resourcespath", &LuaClass_PlanetConfig::LUA_setresourcespath },
    { "set_resourcesready", &LuaClass_PlanetConfig::LUA_setresourcesready },    
    { "set_planetray", &LuaClass_PlanetConfig::LUA_setplanetray },
    { "set_gravityacc", &LuaClass_PlanetConfig::LUA_setgravityacc },
    { "set_atmothickness", &LuaClass_PlanetConfig::LUA_setatmothickness },
    { "set_flatcloudsaltitude", &LuaClass_PlanetConfig::LUA_setflatcloudsaltitude },
    { "set_amplitudes", &LuaClass_PlanetConfig::LUA_setamplitudes },
    { "set_offsets", &LuaClass_PlanetConfig::LUA_setoffsets },
    { "set_seeds", &LuaClass_PlanetConfig::LUA_setseeds },
    { "set_splattingparams", &LuaClass_PlanetConfig::LUA_setsplattingparams },
    { "set_zbufferactivationrelalt", &LuaClass_PlanetConfig::LUA_setzbufferactivationrelalt },
    { "set_fogandatmoparams", &LuaClass_PlanetConfig::LUA_setfogandatmoparams },
    { "set_terrainbumpfactor", &LuaClass_PlanetConfig::LUA_setterrainbumpfactor },
    { "set_beachlimit", &LuaClass_PlanetConfig::LUA_setbeachlimit },
    { "enable_oceans", &LuaClass_PlanetConfig::LUA_enableoceans },
    { "set_climateshaders", &LuaClass_PlanetConfig::LUA_setclimateshaders },
    { "set_climateshaderscompiled", &LuaClass_PlanetConfig::LUA_setclimateshaderscompiled },
    { "set_collisionshaders", &LuaClass_PlanetConfig::LUA_setcollisionshaders },
    { "set_collisionshaderscompiled", &LuaClass_PlanetConfig::LUA_setcollisionshaderscompiled },
    { "enable_collisiondisplay", &LuaClass_PlanetConfig::LUA_enablecollisiondisplay },
    { "set_collisiondisplayshaders", &LuaClass_PlanetConfig::LUA_setcollisiondisplayshaders },
    { "set_collisiondisplayshaderscompiled", &LuaClass_PlanetConfig::LUA_setcollisiondisplayshaderscompiled },
    { "enable_landplacepatch", &LuaClass_PlanetConfig::LUA_enablelandplacepatch },
    { "enable_atmosphere", &LuaClass_PlanetConfig::LUA_enableatmosphere },
    { "enable_light", &LuaClass_PlanetConfig::LUA_enablelight },
    { "set_lightcolor", &LuaClass_PlanetConfig::LUA_setlightcolor },
    { "set_lightdir", &LuaClass_PlanetConfig::LUA_setlightdir },
    { "set_bumppass", &LuaClass_PlanetConfig::LUA_setbumppass },
    { "set_reflectionpass", &LuaClass_PlanetConfig::LUA_setreflectionpass },
    { "set_mainpass", &LuaClass_PlanetConfig::LUA_setmainpass },
    { "set_oceanmaskpass", &LuaClass_PlanetConfig::LUA_setoceanmaskpass },
    { "set_wavepassresol", &LuaClass_PlanetConfig::LUA_setwavepassresol },
    { "set_oceanbumpfactor", &LuaClass_PlanetConfig::LUA_setoceanbumpfactor },


    { "connect_wavepass", &LuaClass_PlanetConfig::LUA_connectwavepass },
    

    { "get_outparam", &LuaClass_PlanetConfig::LUA_getoutparam },
    { 0, 0 }
};

LuaClass_PlanetConfig::LuaClass_PlanetConfig(lua_State* p_L) :
m_rendering_aspect( NULL )
{
    m_planets_details.enable_landplace_patch = false;
    m_planets_details.enable_atmosphere = false;
    m_planets_details.resources_ready = false;

    // prepare lights tuple
    for( int i = 0; i < 4; i++ )
    {
        std::array<dsreal, 3> light_color = { 1.0, 1.0, 1.0 };
        std::array <dsreal, 3> light_dir = { 0.0, 1.0, 0.0 };
        m_planets_details.lights[i] = std::make_tuple( false, light_color, light_dir);
    }
}

LuaClass_PlanetConfig::~LuaClass_PlanetConfig(void)
{
}

int LuaClass_PlanetConfig::LUA_apply(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::apply : argument(s) missing");
    }

    LuaClass_PlanetRendering* planet_rendering{ Luna<LuaClass_PlanetRendering>::check(p_L, 1) };
    DrawSpace::Aspect::RenderingAspect* entity_rendering_aspect{ planet_rendering->GetRenderingAspect() };

    entity_rendering_aspect->AddComponent<dsstring>("resources_path", m_planets_details.resources_path);
    entity_rendering_aspect->AddComponent<bool>("resources_ready", m_planets_details.resources_ready);
    entity_rendering_aspect->AddComponent<dsreal>("planet_ray", m_planets_details.planet_ray);
    entity_rendering_aspect->AddComponent<dsreal>("gravity_acc", m_planets_details.gravity_acc);
    entity_rendering_aspect->AddComponent<dsreal>("atmo_thickness", m_planets_details.atmo_thickness);
    entity_rendering_aspect->AddComponent<dsreal>("flatclouds_altitude", m_planets_details.flatclouds_altitude);
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

    entity_rendering_aspect->AddComponent<dsreal>("zbufferactivationrelalt", m_planets_details.zbuffer_activation_relative_alt);

    entity_rendering_aspect->AddComponent<int>("splat_texture_resol", m_planets_details.splat_texture_resol);

    entity_rendering_aspect->AddComponent<dsreal>("atmo_kr", m_planets_details.atmo_kr);
    entity_rendering_aspect->AddComponent<dsreal>("fog_alt_limit", m_planets_details.fog_alt_limit);
    entity_rendering_aspect->AddComponent<dsreal>("fog_density", m_planets_details.fog_density);
    entity_rendering_aspect->AddComponent<dsreal>("beach_limit", m_planets_details.beach_limit);

    entity_rendering_aspect->AddComponent<bool>("oceans", m_planets_details.oceans);

    entity_rendering_aspect->AddComponent<bool>("enable_landplace_patch", m_planets_details.enable_landplace_patch);
    entity_rendering_aspect->AddComponent<bool>("enable_atmosphere", m_planets_details.enable_atmosphere);

    entity_rendering_aspect->AddComponent<dsstring>("bump_pass", m_planets_details.bump_pass);
    entity_rendering_aspect->AddComponent<dsstring>("reflection_pass", m_planets_details.reflection_pass);
    entity_rendering_aspect->AddComponent<dsstring>("main_pass", m_planets_details.main_pass);
    entity_rendering_aspect->AddComponent<dsstring>("oceanmask_pass", m_planets_details.oceanmask_pass);

    std::pair<dsstring, dsstring> climate_shaders(m_planets_details.climate_vshader, m_planets_details.climate_pshader);
    entity_rendering_aspect->AddComponent<std::pair<dsstring,dsstring>>("climate_shaders", climate_shaders);

    std::pair<bool, bool> climate_shaders_compiled(m_planets_details.climate_vshader_compiled, m_planets_details.climate_pshader_compiled);
    entity_rendering_aspect->AddComponent<std::pair<bool, bool>>("climate_shaders_compiled", climate_shaders_compiled);

    std::pair<dsstring, dsstring> collisions_shaders(m_planets_details.collisions_vshader, m_planets_details.collisions_pshader);
    entity_rendering_aspect->AddComponent<std::pair<dsstring, dsstring>>("collisions_shaders", collisions_shaders);

    std::pair<bool, bool> collisions_shaders_compiled(m_planets_details.collisions_vshader_compiled, m_planets_details.collisions_pshader_compiled);
    entity_rendering_aspect->AddComponent<std::pair<bool, bool>>("collisions_shaders_compiled", collisions_shaders_compiled);


    entity_rendering_aspect->AddComponent<bool>("enable_collisionmeshe_display", m_planets_details.enable_collisionmeshe_display);

    std::pair<dsstring, dsstring> collisionmeshe_display_shaders(m_planets_details.collisionmeshe_display_vshader, m_planets_details.collisionmeshe_display_pshader);
    entity_rendering_aspect->AddComponent<std::pair<dsstring, dsstring>>("collisionmeshe_display_shaders", collisionmeshe_display_shaders);

    std::pair<bool, bool> collisionmeshe_display_shaders_compiled(m_planets_details.collisionmeshe_display_vshader_compiled, m_planets_details.collisionmeshe_display_pshader_compiled);
    entity_rendering_aspect->AddComponent<std::pair<bool, bool>>("collisionmeshe_display_shaders_compiled", collisionmeshe_display_shaders_compiled);

    entity_rendering_aspect->AddComponent<int>("wave_pass_resol", m_planets_details.wave_pass_resol);
    entity_rendering_aspect->AddComponent<dsreal>("ocean_bump_factor", m_planets_details.ocean_bump_factor);


    std::vector<PlanetDetails::Lights> lights;
    for( int i = 0; i < 4; i++ )
    {
        lights.push_back(m_planets_details.lights[i] );
    }
    entity_rendering_aspect->AddComponent<std::vector<PlanetDetails::Lights>>("lights", lights);
   
    ///////////////////////
    // OUT params

    // subpass queues activity
    entity_rendering_aspect->AddComponent<int>("OUT_delayedSingleSubPassQueueSize");

    // cameras infos
    entity_rendering_aspect->AddComponent<ViewOutInfos>("OUT_viewsInfos");


    m_rendering_aspect = entity_rendering_aspect;

    return 0;
}

int LuaClass_PlanetConfig::LUA_cleanup(lua_State* p_L)
{
    if (NULL == m_rendering_aspect)
    {
        _DSEXCEPTION("rendering aspect not set !!!");
    }

    m_rendering_aspect->RemoveComponent<dsstring>("resources_path");
    m_rendering_aspect->RemoveComponent<bool>("resources_ready");
    m_rendering_aspect->RemoveComponent<dsreal>("planet_ray");
    m_rendering_aspect->RemoveComponent<dsreal>("gravity_acc");
    m_rendering_aspect->RemoveComponent<dsreal>("atmo_thickness");
    m_rendering_aspect->RemoveComponent<dsreal>("flatclouds_altitude");
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
    m_rendering_aspect->RemoveComponent<dsreal>("zbufferactivationrelalt");
    m_rendering_aspect->RemoveComponent<int>("splat_texture_resol");
    m_rendering_aspect->RemoveComponent<dsreal>("atmo_kr");
    m_rendering_aspect->RemoveComponent<dsreal>("fog_alt_limit");
    m_rendering_aspect->RemoveComponent<dsreal>("fog_density");
    m_rendering_aspect->RemoveComponent<dsreal>("beach_limit");
    m_rendering_aspect->RemoveComponent<bool>("oceans");
    m_rendering_aspect->RemoveComponent<bool>("enable_landplace_patch");
    m_rendering_aspect->RemoveComponent<bool>("enable_atmosphere");
    m_rendering_aspect->RemoveComponent<dsstring>("bump_pass");
    m_rendering_aspect->RemoveComponent<dsstring>("reflection_pass");
    m_rendering_aspect->RemoveComponent<dsstring>("main_pass");
    m_rendering_aspect->RemoveComponent<dsstring>("oceanmask_pass");
    m_rendering_aspect->RemoveComponent<std::pair<dsstring, dsstring>>("climate_shaders");
    m_rendering_aspect->RemoveComponent<std::pair<bool, bool>>("climate_shaders_compiled");
    m_rendering_aspect->RemoveComponent<std::pair<dsstring, dsstring>>("collisions_shaders");
    m_rendering_aspect->RemoveComponent<std::pair<bool, bool>>("collisions_shaders_compiled");
    m_rendering_aspect->RemoveComponent<std::vector<PlanetDetails::Lights>>("lights");

    m_rendering_aspect->RemoveComponent<bool>("enable_collisionmeshe_display");
    m_rendering_aspect->RemoveComponent<std::pair<dsstring, dsstring>>("collisionmeshe_display_shaders");
    m_rendering_aspect->RemoveComponent<std::pair<bool, bool>>("collisionmeshe_display_shaders_compiled");

    m_rendering_aspect->RemoveComponent<int>("wave_pass_resol");
    m_rendering_aspect->RemoveComponent<dsreal>("ocean_bump_factor");

    m_rendering_aspect->RemoveComponent<int>("OUT_delayedSingleSubPassQueueSize");
    m_rendering_aspect->RemoveComponent<ViewOutInfos>("OUT_viewsInfos");

    if (m_rendering_aspect->GetComponent<Texture*>("wavepass_result_texture"))
    {
        m_rendering_aspect->RemoveComponent<Texture*>("wavepass_result_texture");
    }    
    return 0;
}

int LuaClass_PlanetConfig::LUA_updated(lua_State* p_L)
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
    m_rendering_aspect->GetComponent<bool>("resources_ready")->getPurpose() = m_planets_details.resources_ready;

    m_rendering_aspect->GetComponent<dsreal>("ocean_bump_factor")->getPurpose() = m_planets_details.ocean_bump_factor;

    // signaler le chgt d'un ou plusieurs components...
    m_rendering_aspect->ComponentsUpdated();
    return 0;
}

int LuaClass_PlanetConfig::LUA_setresourcespath(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_resourcespath : argument(s) missing");
    }

    m_planets_details.resources_path = luaL_checkstring(p_L, 1);

    return 0;
}

int LuaClass_PlanetConfig::LUA_setresourcesready(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_resourcesready : argument(s) missing");
    }

    m_planets_details.resources_ready = luaL_checkint(p_L, 1);

    return 0;
}

int LuaClass_PlanetConfig::LUA_setplanetray(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_planetray : argument(s) missing");
    }

    m_planets_details.planet_ray = luaL_checknumber(p_L, 1);
    return 0;
}

int LuaClass_PlanetConfig::LUA_setgravityacc(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_gravityacc : argument(s) missing");
    }

    m_planets_details.gravity_acc = luaL_checknumber(p_L, 1);
    return 0;
}

int LuaClass_PlanetConfig::LUA_setatmothickness(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_atmothickness : argument(s) missing");
    }

    m_planets_details.atmo_thickness = luaL_checknumber(p_L, 1);
    return 0;
}

int LuaClass_PlanetConfig::LUA_setflatcloudsaltitude(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_flatcloudsaltitude : argument(s) missing");
    }

    m_planets_details.flatclouds_altitude = luaL_checknumber(p_L, 1);
    return 0;
}

int LuaClass_PlanetConfig::LUA_setamplitudes(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("PlanetConfig::set_amplitudes : argument(s) missing");
    }

    m_planets_details.plains_amplitude = luaL_checknumber(p_L, 1);
    m_planets_details.mountains_amplitude = luaL_checknumber(p_L, 2);

    return 0;
}

int LuaClass_PlanetConfig::LUA_setoffsets(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("PlanetConfig::set_offsets : argument(s) missing");
    }

    m_planets_details.vertical_offset = luaL_checknumber(p_L, 1);
    m_planets_details.mountains_offset = luaL_checknumber(p_L, 2);

    return 0;
}

int LuaClass_PlanetConfig::LUA_setseeds(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 4)
    {
        LUA_ERROR("PlanetConfig::set_seeds : argument(s) missing");
    }

    m_planets_details.plains_seed1 = luaL_checknumber(p_L, 1);
    m_planets_details.plains_seed2 = luaL_checknumber(p_L, 2);
    m_planets_details.mix_seed1 = luaL_checknumber(p_L, 3);
    m_planets_details.mix_seed2 = luaL_checknumber(p_L, 4);

    return 0;
}

int LuaClass_PlanetConfig::LUA_setsplattingparams(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 3)
    {
        LUA_ERROR("PlanetConfig::set_splattingparams : argument(s) missing");
    }

    m_planets_details.splat_transition_up_relative_alt = luaL_checknumber(p_L, 1);
    m_planets_details.splat_transition_down_relative_alt = luaL_checknumber(p_L, 2);
    m_planets_details.splat_texture_resol = luaL_checknumber(p_L, 3);

    return 0;
}

int LuaClass_PlanetConfig::LUA_setzbufferactivationrelalt(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_zbufferactivationrelalt : argument(s) missing");
    }

    m_planets_details.zbuffer_activation_relative_alt = luaL_checknumber(p_L, 1);

    return 0;
}

int LuaClass_PlanetConfig::LUA_setfogandatmoparams(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 3)
    {
        LUA_ERROR("PlanetConfig::set_fogandatmoparams : argument(s) missing");
    }

    m_planets_details.atmo_kr = luaL_checknumber(p_L, 1);
    m_planets_details.fog_alt_limit = luaL_checknumber(p_L, 2);
    m_planets_details.fog_density = luaL_checknumber(p_L, 3);

    return 0;
}

int LuaClass_PlanetConfig::LUA_setterrainbumpfactor(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_terrainbumpfactor : argument(s) missing");
    }

    m_planets_details.terrainbump_factor = luaL_checknumber(p_L, 1);
    return 0;
}

int LuaClass_PlanetConfig::LUA_setbeachlimit(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_beachlimit : argument(s) missing");
    }

    m_planets_details.beach_limit = luaL_checknumber(p_L, 1);
    return 0;
}

int LuaClass_PlanetConfig::LUA_enableoceans(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::enable_oceans : argument(s) missing");
    }

    m_planets_details.oceans = luaL_checkint(p_L, 1);
    return 0;
}


int LuaClass_PlanetConfig::LUA_enableatmosphere(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::enable_atmosphere : argument(s) missing");
    }

    m_planets_details.enable_atmosphere = luaL_checkint(p_L, 1);
    return 0;
}

int LuaClass_PlanetConfig::LUA_enablelandplacepatch(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::enable_landplacepatch : argument(s) missing");
    }

    m_planets_details.enable_landplace_patch = luaL_checkint(p_L, 1);
    return 0;
}

int LuaClass_PlanetConfig::LUA_setclimateshaders(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("PlanetConfig::set_climateshaders : argument(s) missing");
    }

    m_planets_details.climate_vshader = luaL_checkstring(p_L, 1);
    m_planets_details.climate_pshader = luaL_checkstring(p_L, 2);

    return 0;
}

int LuaClass_PlanetConfig::LUA_setclimateshaderscompiled(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("PlanetConfig::set_climateshaderscompiled : argument(s) missing");
    }

    m_planets_details.climate_vshader_compiled = luaL_checkint(p_L, 1);
    m_planets_details.climate_pshader_compiled = luaL_checkint(p_L, 2);

    return 0;
}

int LuaClass_PlanetConfig::LUA_setcollisionshaders(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("PlanetConfig::set_collisionshaders : argument(s) missing");
    }

    m_planets_details.collisions_vshader = luaL_checkstring(p_L, 1);
    m_planets_details.collisions_pshader = luaL_checkstring(p_L, 2);

    return 0;
}

int LuaClass_PlanetConfig::LUA_setcollisionshaderscompiled(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("PlanetConfig::set_collisionshaderscompiled : argument(s) missing");
    }

    m_planets_details.collisions_vshader_compiled = luaL_checkint(p_L, 1);
    m_planets_details.collisions_pshader_compiled = luaL_checkint(p_L, 2);

    return 0;
}


int LuaClass_PlanetConfig::LUA_enablecollisiondisplay(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::enable_collisiondisplay : argument(s) missing");
    }

    m_planets_details.enable_collisionmeshe_display = luaL_checkint(p_L, 1);

    return 0;
}


int LuaClass_PlanetConfig::LUA_setcollisiondisplayshaders(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("PlanetConfig::set_collisiondisplayshaders : argument(s) missing");
    }

    m_planets_details.collisionmeshe_display_vshader = luaL_checkstring(p_L, 1);
    m_planets_details.collisionmeshe_display_pshader = luaL_checkstring(p_L, 2);

    return 0;
}

int LuaClass_PlanetConfig::LUA_setcollisiondisplayshaderscompiled(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("PlanetConfig::set_collisiondisplayshaderscompiled : argument(s) missing");
    }

    m_planets_details.collisionmeshe_display_vshader_compiled = luaL_checkint(p_L, 1);
    m_planets_details.collisionmeshe_display_pshader_compiled = luaL_checkint(p_L, 2);

    return 0;
}

int LuaClass_PlanetConfig::LUA_enablelight(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("PlanetConfig::enable_light : argument(s) missing");
    }

    int light_index = luaL_checkint(p_L, 1);
    bool light_state = luaL_checkint(p_L, 2);

    std::get<0>(m_planets_details.lights[light_index]) = light_state;
    return 0;
}

int LuaClass_PlanetConfig::LUA_setlightcolor(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 4)
    {
        LUA_ERROR("PlanetConfig::set_lightcolor : argument(s) missing");
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

int LuaClass_PlanetConfig::LUA_setlightdir(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 4)
    {
        LUA_ERROR("PlanetConfig::set_lightdir : argument(s) missing");
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

int LuaClass_PlanetConfig::LUA_setreflectionpass(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_reflectionpass : argument(s) missing");
    }
    dsstring passid = luaL_checkstring(p_L, 1);
    m_planets_details.reflection_pass = passid;
    return 0;
}

int LuaClass_PlanetConfig::LUA_setbumppass(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_bumppass : argument(s) missing");
    }
    dsstring passid = luaL_checkstring(p_L, 1);
    m_planets_details.bump_pass = passid;
    return 0;
}

int LuaClass_PlanetConfig::LUA_setmainpass(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_mainpass : argument(s) missing");
    }
    dsstring passid = luaL_checkstring(p_L, 1);
    m_planets_details.main_pass = passid;
    return 0;
}

int LuaClass_PlanetConfig::LUA_setoceanmaskpass(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_mainpass : argument(s) missing");
    }
    dsstring passid = luaL_checkstring(p_L, 1);
    m_planets_details.oceanmask_pass = passid;
    return 0;
}

int LuaClass_PlanetConfig::LUA_setwavepassresol(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_wavepassresol : argument(s) missing");
    }
    int resol = luaL_checkint(p_L, 1);
    m_planets_details.wave_pass_resol = resol;
    return 0;
}

int LuaClass_PlanetConfig::LUA_setoceanbumpfactor(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::set_oceanbumpfactor : argument(s) missing");
    }
    dsreal bump_factor = luaL_checknumber(p_L, 1);
    m_planets_details.ocean_bump_factor = bump_factor;
    return 0;
}

int LuaClass_PlanetConfig::LUA_getoutparam(lua_State* p_L)
{
    int nb_ret = 0;
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("PlanetConfig::get_outparam : argument(s) missing");
    }

    dsstring id = luaL_checkstring(p_L, 1);

    if(id == "OUT_delayedSingleSubPassQueueSize")
    {
        lua_pushinteger(p_L, m_rendering_aspect->GetComponent<int>("OUT_delayedSingleSubPassQueueSize")->getPurpose());
        return 1;
    }
    else if( id == "OUT_viewsInfos")
    {
        ViewOutInfos& viewInfos = m_rendering_aspect->GetComponent<ViewOutInfos>("OUT_viewsInfos")->getPurpose();
        
        lua_pushinteger(p_L, viewInfos.size()); nb_ret++;
        for(auto& e : viewInfos)
        {
            dsstring camera_name = e.first;
            int currentLOD = std::get<0>(e.second);
            int relative = std::get<1>(e.second);
            dsreal rel_alt = std::get<2>(e.second);
            dsreal alt = std::get<3>(e.second);
            dsreal current_patch_max_height = std::get<4>(e.second);
            dsreal current_patch_min_height = std::get<5>(e.second);
            dsreal current_patch_current_height = std::get<6>(e.second);
            Vector locale_camera_pos{ std::get<7>(e.second) };
            Vector global_camera_pos{ std::get<8>(e.second) };

            lua_pushstring(p_L, camera_name.c_str()); nb_ret++;
            lua_pushinteger(p_L, currentLOD); nb_ret++;
            lua_pushinteger(p_L, relative); nb_ret++;
            lua_pushnumber(p_L, rel_alt); nb_ret++;
            lua_pushnumber(p_L, alt); nb_ret++;

            lua_pushnumber(p_L, current_patch_max_height); nb_ret++;
            lua_pushnumber(p_L, current_patch_min_height); nb_ret++;
            lua_pushnumber(p_L, current_patch_current_height); nb_ret++;

            lua_pushnumber(p_L, locale_camera_pos[0]); nb_ret++;
            lua_pushnumber(p_L, locale_camera_pos[1]); nb_ret++;
            lua_pushnumber(p_L, locale_camera_pos[2]); nb_ret++;

            lua_pushnumber(p_L, global_camera_pos[0]); nb_ret++;
            lua_pushnumber(p_L, global_camera_pos[1]); nb_ret++;
            lua_pushnumber(p_L, global_camera_pos[2]); nb_ret++;
        }
    }
    else
    {
        LUA_ERROR("PlanetConfig::get_outparam : Unknown param id : " << id);
    }    
    return nb_ret;
}

int LuaClass_PlanetConfig::LUA_connectwavepass(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 3)
    {
        LUA_ERROR("PlanetConfig::connect_wavepass : argument(s) missing");
    }

    LuaClass_RenderPassNodeGraph* rg = Luna<LuaClass_RenderPassNodeGraph>::check(p_L, 1);
    dsstring pass_wave_id = luaL_checkstring(p_L, 2);

    LuaClass_PlanetRendering* lua_planetrendering = Luna<LuaClass_PlanetRendering>::check(p_L, 3);
    DrawSpace::Aspect::RenderingAspect* entity_rendering_aspect{ lua_planetrendering->GetRenderingAspect() };

    Texture* target_texture{ rg->GetNode(pass_wave_id).GetTargetTexture() };     
    entity_rendering_aspect->AddComponent<Texture*>("wavepass_result_texture", target_texture);

    return 0;
}