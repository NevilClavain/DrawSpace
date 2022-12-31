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

#include "luna.h"
#include "rigidbodytransformaspectimpl.h"
#include "transformaspect.h"

class LuaClass_RigidBodyTransform
{
private:

    DrawSpace::AspectImplementations::RigidBodyTransformAspectImpl      m_rigidbody_transformer;
    DrawSpace::Aspect::TransformAspect*                                 m_entity_transform_aspect{ nullptr };
    DrawSpace::Core::Entity*                                            m_entity{ nullptr };

    int                                                                 m_shape_type{ -1 };

    bool                                                                m_attitude_setted{ false };
    bool                                                                m_mass_setted{ false };

    std::vector<dsstring>                                               m_forces_id;
    std::vector<dsstring>                                               m_torques_id;


public:

    LuaClass_RigidBodyTransform(lua_State* p_L);
    ~LuaClass_RigidBodyTransform(void);

    int LUA_attachtoentity(lua_State* p_L);

    int LUA_configureshape(lua_State* p_L);
    int LUA_configureattitude(lua_State* p_L);
    int LUA_configuremass(lua_State* p_L);
    int LUA_configureforce(lua_State* p_L);
    int LUA_configuretorque(lua_State* p_L);

    int LUA_updateattitude(lua_State* p_L);

    int LUA_updateforce(lua_State* p_L);
    int LUA_updateforcestate(lua_State* p_L);

    int LUA_updatetorque(lua_State* p_L);
    int LUA_updatetorquestate(lua_State* p_L);


    int LUA_zerospeed(lua_State* p_L);
    int LUA_zeroangularspeed(lua_State* p_L);

    int LUA_release(lua_State* p_L);

    static const char className[];
    static const Luna<LuaClass_RigidBodyTransform>::RegType methods[];

};
