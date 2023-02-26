/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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
#include "renderingaspect.h"

// fwd decls
namespace DrawSpace
{
namespace Core
{
class Entity;
class RenderingNode;
}
namespace Aspect
{
class RenderingAspect;
class ResourcesAspect;
}
};

class PlanetsRenderingAspectImpl;
class LuaClass_RenderLayer;
// fwd decls


class LuaClass_PlanetRendering
{
private:

    PlanetsRenderingAspectImpl*         m_planet_render{ nullptr };
    DrawSpace::Aspect::RenderingAspect* m_entity_rendering_aspect{ nullptr };
    DrawSpace::Core::Entity*            m_entity{ nullptr };
    DrawSpace::Utils::TimeManager*      m_tm{ nullptr };

    // table de traduction RenderContext name -> Passes Name
    // permet de savoir a quelle passe est attribue un rendercontext
    std::map<dsstring, std::vector<dsstring>>                                       m_planetlayers_rcname_to_passes;
    std::map<dsstring, std::vector<dsstring>>                                       m_foliagelayers_rcname_to_passes;

    std::map<size_t, dsstring>                                                      m_foliages_meshes_names;
    std::map<size_t, DrawSpace::Core::Meshe>                                        m_foliages_meshes;

    void cleanup_resources(lua_State* p_L);

    void configure_from_renderlayer(lua_State* p_L,  LuaClass_RenderLayer* p_lua_renderlayer, DrawSpace::Aspect::ResourcesAspect* p_resources_aspect, const dsstring& p_comp_prefix);
    
public:

    LuaClass_PlanetRendering( lua_State* p_L );
	~LuaClass_PlanetRendering( void );

    int LUA_attachtoentity(lua_State* p_L);
    int LUA_detachfromentity(lua_State* p_L);

    int LUA_setPassForPlanetLayerRenderId(lua_State* p_L);
    int LUA_setPassForFoliageLayerRenderId(lua_State* p_L);

    int LUA_configure(lua_State* p_L);
    int LUA_release(lua_State* p_L);

    int LUA_registertorendering(lua_State* p_L);
    int LUA_unregisterfromrendering(lua_State* p_L);

    int LUA_declarefoliagemeshe(lua_State* p_L);



    DrawSpace::Aspect::RenderingAspect* GetRenderingAspect(void) const;
    
    static const char className[];
    static const Luna<LuaClass_PlanetRendering>::RegType methods[];
};
