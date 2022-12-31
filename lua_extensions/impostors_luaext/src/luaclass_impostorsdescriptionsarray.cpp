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

std::vector<ImpostorsRenderingAspectImpl::ImpostorDescriptor> LuaClass_ImpostorsDescriptionsArray::GetData(void) const
{
    return m_descriptions;
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
    int argc{ lua_gettop(p_L) };
    if (argc < 3)
    {
        LUA_ERROR("ImpostorsDescriptionsArray::set_scale : argument(s) missing");
    }
    
    int index{ luaL_checkint(p_L, 1) };
    dsreal width_scale{ luaL_checknumber(p_L, 2) };
    dsreal height_scale{ luaL_checknumber(p_L, 3) };

    ImpostorsRenderingAspectImpl::ImpostorDescriptor descriptor{ m_descriptions[index] };
    descriptor.width_scale = width_scale;
    descriptor.height_scale = height_scale;
    m_descriptions[index] = descriptor;

    return 0;
}

int LuaClass_ImpostorsDescriptionsArray::LUA_setposition(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 4)
    {
        LUA_ERROR("ImpostorsDescriptionsArray::set_position : argument(s) missing");
    }

    int index{ luaL_checkint(p_L, 1) };
    dsreal x{ luaL_checknumber(p_L, 2) };
    dsreal y{ luaL_checknumber(p_L, 3) };
    dsreal z{ luaL_checknumber(p_L, 4) };

    ImpostorsRenderingAspectImpl::ImpostorDescriptor descriptor{ m_descriptions[index] };
    descriptor.localpos[0] = x;
    descriptor.localpos[1] = y;
    descriptor.localpos[2] = z;
    m_descriptions[index] = descriptor;

    return 0;
}

int LuaClass_ImpostorsDescriptionsArray::LUA_setuvcoords(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 9)
    {
        LUA_ERROR("ImpostorsDescriptionsArray::set_uvcoords : argument(s) missing");
    }

    int index{ luaL_checkint(p_L, 1) };
    dsreal u1{ luaL_checknumber(p_L, 2) };
    dsreal v1{ luaL_checknumber(p_L, 3) };
    dsreal u2{ luaL_checknumber(p_L, 4) };
    dsreal v2{ luaL_checknumber(p_L, 5) };
    dsreal u3{ luaL_checknumber(p_L, 6) };
    dsreal v3{ luaL_checknumber(p_L, 7) };
    dsreal u4{ luaL_checknumber(p_L, 8) };
    dsreal v4{ luaL_checknumber(p_L, 9) };

    ImpostorsRenderingAspectImpl::ImpostorDescriptor descriptor{ m_descriptions[index] };
    
    descriptor.u1 = u1;
    descriptor.v1 = v1;
    descriptor.u2 = u2;
    descriptor.v2 = v2;
    descriptor.u3 = u3;
    descriptor.v3 = v3;
    descriptor.u4 = u4;
    descriptor.v4 = v4;

    m_descriptions[index] = descriptor;

    return 0;
}
