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

#ifndef _LUACLASS_MATRIX_H_
#define _LUACLASS_MATRIX_H_

#include "luna.h"
#include "matrix.h"

class LuaClass_Matrix
{
protected:

    DrawSpace::Utils::Matrix        m_matrix;

public:
	LuaClass_Matrix( lua_State* p_L );
	~LuaClass_Matrix( void );

    int LUA_zero( lua_State* p_L );
    int LUA_identity( lua_State* p_L );
    int LUA_translation( lua_State* p_L );
    int LUA_transpose( lua_State* p_L );
    int LUA_perspective( lua_State* p_L );
    int LUA_scale( lua_State* p_L );
    int LUA_cleartranslation( lua_State* p_L );
    int LUA_rotation( lua_State* p_L );
    int LUA_inverse( lua_State* p_L );
	int LUA_rotationfromquaternion(lua_State* p_L);

    int LUA_setvalue( lua_State* p_L );
    int LUA_getvalue( lua_State* p_L );

    int LUA_storeproduct( lua_State* p_L );

    DrawSpace::Utils::Matrix GetMatrix( void ) const;
    void SetMatrix(const DrawSpace::Utils::Matrix& p_mat);

	int LUA_storeentitytransformation(lua_State* p_L);

    static const char className[];
    static const Luna<LuaClass_Matrix>::RegType methods[];
};

#endif
