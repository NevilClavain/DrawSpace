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
#include "drawspace_commons.h"

#define __LUAEXT_DESCRIPTION__( __description_text__ ) dsstring Description(void) const { return __description_text__; };

//fwd decl
struct lua_State;
namespace DrawSpace
{
namespace Utils
{
class MemAlloc;
}
namespace Interface
{
class Renderer;
}
namespace Systems
{
class Hub;
}
namespace Logger
{
class Configuration;
}
}
class LuaExtension
{
public:
	virtual void Register(lua_State*) = 0;
	virtual void UpdateRenderer(DrawSpace::Interface::Renderer*) = 0;	
	virtual dsstring Description(void) const = 0;
	virtual DrawSpace::Utils::MemAlloc* GetMemAllocInstance(void) const = 0;

	virtual void SetLoggerConfiguration(DrawSpace::Logger::Configuration* p_logconf) {};

	virtual void SetHub(DrawSpace::Systems::Hub* p_hub) {};
};
