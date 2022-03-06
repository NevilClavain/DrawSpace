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
#include "luaclass_planetrendering.h"
#include "luaclass_entity.h"
#include "luaclass_timemanagerref.h"
#include "planetsrenderingaspectimpl.h"
#include "planetsluaext.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;


const char LuaClass_PlanetRendering::className[] = "PlanetRendering";
const Luna<LuaClass_PlanetRendering>::RegType LuaClass_PlanetRendering::methods[] =
{
	{ "attach_toentity", &LuaClass_PlanetRendering::LUA_attachtoentity },
    { "detach_fromentity", &LuaClass_PlanetRendering::LUA_detachfromentity },
    { "set_passforrenderid", &LuaClass_PlanetRendering::LUA_setPassForRenderId },
	{ 0, 0 }
};

LuaClass_PlanetRendering::LuaClass_PlanetRendering( lua_State* p_L )
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Rendering::Rendering : argument(s) missing");
    }
    LuaClass_TimeManagerRef* lua_tmref = Luna<LuaClass_TimeManagerRef>::check(p_L, 1);
    m_tm = lua_tmref->GetTimeManager();
}

LuaClass_PlanetRendering::~LuaClass_PlanetRendering( void )
{
}

int LuaClass_PlanetRendering::LUA_attachtoentity(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 1)
    {
        LUA_ERROR("PlanetRendering::attach_toentity : argument(s) missing");
    }

    LuaClass_Entity* lua_ent{ Luna<LuaClass_Entity>::check(p_L, 1) };
    DrawSpace::Core::Entity& entity{ lua_ent->GetEntity() };

    RenderingAspect* rendering_aspect{ entity.GetAspect<RenderingAspect>() };
    if (NULL == rendering_aspect)
    {
        LUA_ERROR("PlanetRendering::attach_toentity : entity has no rendering aspect!");
    }

    m_entity_rendering_aspect = rendering_aspect;
    m_entity = &entity;

    m_planet_render = _DRAWSPACE_NEW_(PlanetsRenderingAspectImpl, PlanetsRenderingAspectImpl);
    m_planet_render->SetHub(PlanetsLuaExtension::GetInstance()->GetHub());
    m_entity_rendering_aspect->AddImplementation(m_planet_render, m_tm);

    return 0;
}

int LuaClass_PlanetRendering::LUA_detachfromentity(lua_State* p_L)
{
    if (!m_entity)
    {
        LUA_ERROR("PlanetRendering::detach_fromentity : argument(s) missing");
    }
    if (m_planet_render)
    {
        _DRAWSPACE_DELETE_(m_planet_render);
    }
    LUA_TRY
    {
        m_entity_rendering_aspect->RemoveImplementation(m_planet_render);

    } LUA_CATCH;

    m_entity_rendering_aspect = nullptr;
    m_entity = nullptr;
    return 0;
}

int LuaClass_PlanetRendering::LUA_setPassForRenderId(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 2)
    {
        LUA_ERROR("PlanetRendering::set_passforrenderid : argument(s) missing");
    }

    dsstring rc_id{ luaL_checkstring(p_L, 1) };
    dsstring pass_id{ luaL_checkstring(p_L, 2) };
    m_rcname_to_passes[rc_id].push_back(pass_id);

    return 0;
}