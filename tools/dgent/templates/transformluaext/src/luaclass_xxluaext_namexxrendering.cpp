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
#include "luaclass_??luaext_name??rendering.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;

const char LuaClass_??class_name??Rendering::className[] = "??class_name??Rendering";
const Luna<LuaClass_??class_name??Rendering>::RegType LuaClass_??class_name??Rendering::methods[] =
{
    { "configure", &LuaClass_??class_name??Rendering::LUA_configure },
    { "release", &LuaClass_??class_name??Rendering::LUA_release },
	{ 0, 0 }
};

LuaClass_??class_name??Rendering::LuaClass_??class_name??Rendering( lua_State* p_L )
{
}

LuaClass_??class_name??Rendering::~LuaClass_??class_name??Rendering( void )
{
}


int LuaClass_??class_name??Rendering::LUA_configure( lua_State* p_L )
{
    return 0;
}

int LuaClass_??class_name??Rendering::LUA_release( lua_State* p_L )
{
    if( !m_entity_rendering_aspect )
    {
        LUA_ERROR( "??class_name??Rendering::update : no rendering aspect" );
    }
		
    return 0;
}