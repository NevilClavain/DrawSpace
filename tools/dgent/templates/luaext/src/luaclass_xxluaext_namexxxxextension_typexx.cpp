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
#include "luaclass_??luaext_name????extension_type??.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;

const char LuaClass_??class_name????extension_type??::className[] = "??class_name????extension_type??";
const Luna<LuaClass_??class_name????extension_type??>::RegType LuaClass_??class_name????extension_type??::methods[] =
{
    { "configure", &LuaClass_??class_name????extension_type??::LUA_configure },
    { "update", &LuaClass_??class_name????extension_type??::LUA_update },
    { "release", &LuaClass_??class_name????extension_type??::LUA_release },
	{ 0, 0 }
};

LuaClass_??class_name????extension_type??::LuaClass_??class_name????extension_type??( lua_State* p_L ) :
{
}

LuaClass_??class_name????extension_type??::~LuaClass_??class_name????extension_type??( void )
{
}


int LuaClass_??class_name????extension_type??::LUA_configure( lua_State* p_L )
{
    return 0;
}

int LuaClass_??class_name????extension_type??::LUA_release( lua_State* p_L )
{
    if( !m_entity_??extension_type??_aspect )
    {
        LUA_ERROR( "??class_name????extension_type??::update : no rendering aspect" );
    }
		
    return 0;
}

int LuaClass_??class_name????extension_type??::LUA_update( lua_State* p_L )
{
    if( !m_entity_??extension_type??_aspect )
    {
        LUA_ERROR( "??class_name????extension_type??::update : no rendering aspect" );
    }

    return 0;
}
