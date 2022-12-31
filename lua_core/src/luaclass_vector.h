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
#include "vector.h"

class LuaClass_Vector
{
protected:
    std::unique_ptr<DrawSpace::Utils::Vector>        m_vector;

public:
    LuaClass_Vector(lua_State* p_L);
    ~LuaClass_Vector(void);

    int LUA_getx(lua_State* p_L);
    int LUA_gety(lua_State* p_L);
    int LUA_getz(lua_State* p_L);
    int LUA_getw(lua_State* p_L);

    int LUA_setx(lua_State* p_L);
    int LUA_sety(lua_State* p_L);
    int LUA_setz(lua_State* p_L);
    int LUA_setw(lua_State* p_L);

    int LUA_lengthpow2(lua_State* p_L);
    int LUA_length(lua_State* p_L);

    int LUA_normalize(lua_State* p_L);
    int LUA_scale(lua_State* p_L);

    int LUA_dotproductwith(lua_State* p_L);
    int LUA_crossproductwith(lua_State* p_L);
    int LUA_copy(lua_State* p_L);
    int LUA_addwith(lua_State* p_L);
    int LUA_subwith(lua_State* p_L);

    DrawSpace::Utils::Vector getVector( void ) const;
    void setVector(const DrawSpace::Utils::Vector& p_vector);

    static const char className[];
    static const Luna<LuaClass_Vector>::RegType methods[];
};

