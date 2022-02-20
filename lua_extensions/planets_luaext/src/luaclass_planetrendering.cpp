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


using namespace DrawSpace;

const char LuaClass_PlanetRendering::className[] = "PlanetRendering";
const Luna<LuaClass_PlanetRendering>::RegType LuaClass_PlanetRendering::methods[] =
{
	{ 0, 0 }
};

LuaClass_PlanetRendering::LuaClass_PlanetRendering( lua_State* p_L )
{
}

LuaClass_PlanetRendering::~LuaClass_PlanetRendering( void )
{
}

