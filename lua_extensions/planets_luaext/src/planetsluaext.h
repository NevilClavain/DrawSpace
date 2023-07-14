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

#include "luaext.h"
#include "crtp_singleton.h"

namespace DrawSpace
{
namespace Logger
{
class Configuration;
}
}

class PlanetsLuaExtension : public LuaExtension, public DrawSpace::Singleton<PlanetsLuaExtension>
{
private:
	DrawSpace::Systems::Hub* m_hub{ nullptr };

public:
	PlanetsLuaExtension(void);

	void						Register(lua_State* p_L);
	void						UpdateRenderer(DrawSpace::Interface::Renderer* p_renderer);
	DrawSpace::Utils::MemAlloc* GetMemAllocInstance(void) const;

	void						SetHub(DrawSpace::Systems::Hub* p_hub);
	DrawSpace::Systems::Hub*	GetHub(void) const;

	void						SetLoggerConfiguration(DrawSpace::Logger::Configuration* p_logconf);


	__LUAEXT_DESCRIPTION__("Procedural planets rendering extension")
};


