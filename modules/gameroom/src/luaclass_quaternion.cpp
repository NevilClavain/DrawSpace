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
#include "luaclass_quaternion.h"
#include "maths.h"
#include "vector.h"


using namespace DrawSpace::Utils;

const char LuaClass_Quaternion::className[] = "Quaternion";
const Luna<LuaClass_Quaternion>::RegType LuaClass_Quaternion::methods[] =
{
	{ "zero", &LuaClass_Quaternion::LUA_zero },
	{ "identity", &LuaClass_Quaternion::LUA_identity },
	{ "rotation_axis", &LuaClass_Quaternion::LUA_rotationaxis },
	{ "normalize", &LuaClass_Quaternion::LUA_normalize },
	{ "store_lerp", &LuaClass_Quaternion::LUA_storelerp },
	{ 0, 0 }
};


LuaClass_Quaternion::LuaClass_Quaternion(lua_State* p_L)
{
}

LuaClass_Quaternion::~LuaClass_Quaternion(void)
{
}

int LuaClass_Quaternion::LUA_zero(lua_State* p_L)
{
	m_quat.Zero();
	return 0;
}

int LuaClass_Quaternion::LUA_identity(lua_State* p_L)
{
	m_quat.Identity();
	return 0;
}

int LuaClass_Quaternion::LUA_rotationaxis(lua_State* p_L)
{
	int argc = lua_gettop(p_L);
	if (argc < 4)
	{
		LUA_ERROR("Quaternion::rotation_axis : argument(s) missing");
	}

	dsreal x = luaL_checknumber(p_L, 1);
	dsreal y = luaL_checknumber(p_L, 2);
	dsreal z = luaL_checknumber(p_L, 3);

	dsreal ang = luaL_checknumber(p_L, 4);

	m_quat.RotationAxis(Vector(x, y, z, 1.0), ang);

	return 0;
}

int LuaClass_Quaternion::LUA_normalize(lua_State* p_L)
{
	m_quat.Normalize();
	return 0;
}

int LuaClass_Quaternion::LUA_storelerp(lua_State* p_L)
{
	int argc = lua_gettop(p_L);
	if (argc < 2)
	{
		LUA_ERROR("Quaternion::store_lerp : argument(s) missing");
	}

	LuaClass_Quaternion* lua_q1 = Luna<LuaClass_Quaternion>::check(p_L, 1);
	LuaClass_Quaternion* lua_q2 = Luna<LuaClass_Quaternion>::check(p_L, 2);

	dsreal blend_factor = luaL_checknumber(p_L, 3);

	m_quat = Quaternion::Lerp(lua_q1->GetQuaternion(), lua_q2->GetQuaternion(), blend_factor);

	return 0;
}

DrawSpace::Utils::Quaternion LuaClass_Quaternion::GetQuaternion(void) const
{
	return m_quat;
}
