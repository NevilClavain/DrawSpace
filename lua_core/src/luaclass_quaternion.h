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

#pragma once

#include "luna.h"
#include "quaternion.h"

class LuaClass_Quaternion
{
protected:

	DrawSpace::Utils::Quaternion        m_quat;

public:
	LuaClass_Quaternion(lua_State* p_L);
	~LuaClass_Quaternion(void);

	int LUA_zero(lua_State* p_L);
	int LUA_identity(lua_State* p_L);
	int LUA_rotationaxis(lua_State* p_L);
	int LUA_normalize(lua_State* p_L);
	int LUA_storelerp(lua_State* p_L);


	DrawSpace::Utils::Quaternion GetQuaternion(void) const;

	static const char className[];
	static const Luna<LuaClass_Quaternion>::RegType methods[];
};


