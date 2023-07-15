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

#include "ds_types.h"
#include "luna.h"

#include "transformaspectimpl.h"

// fwd decls
namespace DrawSpace
{
    namespace Core
    {
        class Entity;
    }
    namespace Aspect
    {
        class TransformAspect;
    }
};

class LuaClass_FreeMovementTransform : public DrawSpace::Interface::AspectImplementations::TransformAspectImpl
{
private:

    DrawSpace::Aspect::TransformAspect*     	m_entity_transform_aspect { nullptr };

public:
    LuaClass_FreeMovementTransform( lua_State* p_L );
	~LuaClass_FreeMovementTransform( void );

    int LUA_configure(lua_State* p_L);
    int LUA_release(lua_State* p_L);
    int LUA_update(lua_State* p_L);
    int LUA_setpos(lua_State* p_L);
    int LUA_lookat(lua_State* p_L);
    int LUA_read(lua_State* p_L);

    int LUA_rotateX(lua_State* p_L);
    int LUA_rotateY(lua_State* p_L);
    int LUA_rotateZ(lua_State* p_L);

    void GetLocaleTransform(DrawSpace::Aspect::TransformAspect* p_transformaspect, DrawSpace::Utils::Matrix& p_out_base_transform);

    static const char className[];
    static const Luna<LuaClass_FreeMovementTransform>::RegType methods[];

};
