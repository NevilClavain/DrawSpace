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
#include "luaclass_impostorsdescriptionsarray.h"


const char LuaClass_ImpostorsDescriptionsArray::className[] = "ImpostorsDescriptionsArray";
const Luna<LuaClass_ImpostorsDescriptionsArray>::RegType LuaClass_ImpostorsDescriptionsArray::methods[] =
{
    { "add", &LuaClass_ImpostorsDescriptionsArray::LUA_add },
    { "set_scale", &LuaClass_ImpostorsDescriptionsArray::LUA_setscale },
    { "set_position", &LuaClass_ImpostorsDescriptionsArray::LUA_setposition },
    { "set_uvcoords", &LuaClass_ImpostorsDescriptionsArray::LUA_setuvcoords },
    { 0, 0 }
};

LuaClass_ImpostorsDescriptionsArray::LuaClass_ImpostorsDescriptionsArray(lua_State* p_L)
{
}

LuaClass_ImpostorsDescriptionsArray::~LuaClass_ImpostorsDescriptionsArray(void)
{
}

int LuaClass_ImpostorsDescriptionsArray::LUA_add(lua_State* p_L)
{
    // create a descriptor with some default values
    ImpostorsRenderingAspectImpl::ImpostorDescriptor descriptor{ 1.0, 1.0, { 0.0, 0.0, 0.0, 1.0 }, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0 };    
    m_descriptions.push_back(descriptor);
    return 0;
}

int LuaClass_ImpostorsDescriptionsArray::LUA_setscale(lua_State* p_L)
{
    return 0;
}

int LuaClass_ImpostorsDescriptionsArray::LUA_setposition(lua_State* p_L)
{
    return 0;
}

int LuaClass_ImpostorsDescriptionsArray::LUA_setuvcoords(lua_State* p_L)
{
    return 0;
}
