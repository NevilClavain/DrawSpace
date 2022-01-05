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
#include "luaclass_syncscalar.h"
#include "luaclass_entity.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;

const char LuaClass_SyncScalar::className[] = "SyncScalar";
const Luna<LuaClass_SyncScalar>::RegType LuaClass_SyncScalar::methods[] =
{
    { "init_fromtimeaspectof", &LuaClass_SyncScalar::LUA_initfromtimeaspectof },
    { "get_value", &LuaClass_SyncScalar::LUA_getvalue },
    { "inc", &LuaClass_SyncScalar::LUA_inc },
    { "dec", &LuaClass_SyncScalar::LUA_dec },
	{ 0, 0 }
};

LuaClass_SyncScalar::LuaClass_SyncScalar( lua_State* p_L )
{

}

LuaClass_SyncScalar::~LuaClass_SyncScalar( void )
{

}


int LuaClass_SyncScalar::LUA_initfromtimeaspectof( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
        LUA_ERROR( "SyncScalar::init_fromtimeaspectof : argument(s) missing" );
	}

    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 1 );

    dsreal init_angle = luaL_checknumber( p_L, 2 );

    TimeAspect* time_aspect = lua_ent->GetEntity().GetAspect<TimeAspect>();
    if( NULL == time_aspect )
    {
        LUA_ERROR( "SyncScalar::init_fromtimeaspectof : entity has no time aspect" );
    }

    m_time_scalar = time_aspect->TimeScalarFactory( init_angle );

    return 0;
}

int LuaClass_SyncScalar::LUA_getvalue( lua_State* p_L )
{
    lua_pushnumber( p_L, m_time_scalar.GetValue() );
    return 1;
}

int LuaClass_SyncScalar::LUA_inc( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "SyncScalar::inc : argument(s) missing" );
	}
    dsreal val = luaL_checknumber( p_L, 1 );
    m_time_scalar.Increase( val );
    return 0;
}

int LuaClass_SyncScalar::LUA_dec( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "SyncScalar::dec : argument(s) missing" );
	}

    dsreal val = luaL_checknumber( p_L, 1 );
    m_time_scalar.Decrease( val );
    return 0;
}
