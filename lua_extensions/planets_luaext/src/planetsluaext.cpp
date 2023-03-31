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

#include "planetsluaext.h"
#include "luaclass_planetrendering.h"
#include "luaclass_planetconfig.h"
#include "plugin.h"
#include "memalloc.h"
#include "hub.h"
#include "logconf.h"
#include "planetscentraladmin.h"

PlanetsLuaExtension::PlanetsLuaExtension(void)
{
}

void PlanetsLuaExtension::Register(lua_State* p_L)
{
	Luna<LuaClass_PlanetRendering>::Register(p_L);
	Luna<LuaClass_PlanetConfig>::Register(p_L);
}

void PlanetsLuaExtension::UpdateRenderer(DrawSpace::Interface::Renderer* p_renderer)
{
	DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface = p_renderer;
}

DrawSpace::Utils::MemAlloc* PlanetsLuaExtension::GetMemAllocInstance(void) const
{
	return DrawSpace::Utils::MemAlloc::GetInstance();
}

void PlanetsLuaExtension::SetHub(DrawSpace::Systems::Hub* p_hub)
{
	m_hub = p_hub;
}

DrawSpace::Systems::Hub* PlanetsLuaExtension::GetHub(void) const
{
	return m_hub;
}

void PlanetsLuaExtension::SetLoggerConfiguration(DrawSpace::Logger::Configuration* p_logconf)
{
	PlanetsCentralAdmin::GetInstance()->SetLogconf(p_logconf);
}