/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2021
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
#include "luaclass_rigidbodytransform.h"
#include "luaclass_entity.h"
#include "timeaspect.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;

const char LuaClass_RigidBodyTransform::className[] = "RigdBodyTransform";
const Luna<LuaClass_RigidBodyTransform>::RegType LuaClass_RigidBodyTransform::methods[] =
{
    { "configure", &LuaClass_RigidBodyTransform::LUA_configure },
    { "release", &LuaClass_RigidBodyTransform::LUA_release },
    { 0, 0 }
};


LuaClass_RigidBodyTransform::LuaClass_RigidBodyTransform(lua_State* p_L)
{

}

LuaClass_RigidBodyTransform::~LuaClass_RigidBodyTransform(void)
{

}

int LuaClass_RigidBodyTransform::LUA_configure(lua_State* p_L)
{
    return 0;
}

int LuaClass_RigidBodyTransform::LUA_release(lua_State* p_L)
{
    return 0;
}
