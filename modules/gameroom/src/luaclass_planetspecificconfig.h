/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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
#include "misc_utils.h"

namespace DrawSpace
{
namespace Aspect
{
class RenderingAspect;
}
}

class LuaClass_PlanetSpecificConfig
{
private:

    // data model

    struct PlanetDetails
    {
        using Lights = std::tuple<bool, std::array<dsreal, 3>, std::array<dsreal, 3>>;

        dsstring    resources_path;
        dsreal      planet_ray; 
        dsreal      atmo_thickness;
        dsreal      plains_amplitude;
        dsreal      mountains_amplitude; 
        dsreal      vertical_offset;
        dsreal      mountains_offset;
        dsreal      plains_seed1; 
        dsreal      plains_seed2; 
        dsreal      mix_seed1; 
        dsreal      mix_seed2;
        dsreal      terrainbump_factor;
        dsreal      splat_transition_up_relative_alt; 
        dsreal      splat_transition_down_relative_alt;
        int         splat_texture_resol;
        dsreal      atmo_kr; 
        dsreal      fog_alt_limit; 
        dsreal      fog_density;
        dsreal      beach_limit;

        dsstring    climate_vshader;
        dsstring    climate_pshader;

        bool        enable_landplace_patch;
        bool        enable_atmosphere;

        Lights      lights[4];

    };

    PlanetDetails m_planets_details;

    DrawSpace::Aspect::RenderingAspect* m_rendering_aspect;

    using ViewOutInfos = std::map<dsstring, std::tuple<int, bool, dsreal, dsreal>>;

public:
    LuaClass_PlanetSpecificConfig(lua_State* p_L);
    ~LuaClass_PlanetSpecificConfig(void);

    int LUA_apply(lua_State* p_L);
    int LUA_cleanup(lua_State* p_L);

    int LUA_updated(lua_State* p_L);

    int LUA_setresourcespath(lua_State* p_L);
    int LUA_setplanetray(lua_State* p_L);
    int LUA_setatmothickness(lua_State* p_L);
    int LUA_setamplitudes(lua_State* p_L);
    int LUA_setoffsets(lua_State* p_L);
    int LUA_setseeds(lua_State* p_L);
    int LUA_setsplattingparams(lua_State* p_L);
    int LUA_setfogandatmoparams(lua_State* p_L);
    int LUA_setterrainbumpfactor(lua_State* p_L);
    int LUA_setbeachlimit(lua_State* p_L);
    int LUA_enablelandplacepatch(lua_State* p_L);
    int LUA_enableatmosphere(lua_State* p_L);
    int LUA_setclimateshaders(lua_State* p_L);

    int LUA_enablelight(lua_State* p_L);
    int LUA_setlightcolor(lua_State* p_L);
    int LUA_setlightdir(lua_State* p_L);


    int LUA_getoutparam(lua_State* p_L);

    static const char className[];
    static const Luna<LuaClass_PlanetSpecificConfig>::RegType methods[];
};
