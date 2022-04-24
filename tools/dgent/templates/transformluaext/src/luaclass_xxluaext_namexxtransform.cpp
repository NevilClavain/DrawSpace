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
#include "luaclass_??luaext_name??transform.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;

const char LuaClass_??class_name??Transform::className[] = "??class_name??Transform";
const Luna<LuaClass_??class_name??Transform>::RegType LuaClass_??class_name??Transform::methods[] =
{
    { "configure", &LuaClass_??class_name??Transform::LUA_configure },
    { "release", &LuaClass_??class_name??Transform::LUA_release },
	{ 0, 0 }
};

LuaClass_??class_name??Transform::LuaClass_??class_name??Transform( lua_State* p_L )
{
}

LuaClass_??class_name??Transform::~LuaClass_??class_name??Transform( void )
{
}


int LuaClass_??class_name??Transform::LUA_configure( lua_State* p_L )
{
    return 0;
}

int LuaClass_??class_name??Transform::LUA_release( lua_State* p_L )
{
    if( !m_entity_transform_aspect )
    {
        LUA_ERROR( "??class_name??Transform::update : no transform aspect" );
    }
		
    return 0;
}