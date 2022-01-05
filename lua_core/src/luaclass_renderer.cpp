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

#include "luaclass_renderer.h"
#include "mainservice.h"
#include "plugin.h"

const char LuaClass_Renderer::className[] = "Renderer";
const Luna<LuaClass_Renderer>::RegType LuaClass_Renderer::methods[] =
{
    { "descr", &LuaClass_Renderer::LUA_getinfos },
	{ 0, 0 }
};

LuaClass_Renderer::LuaClass_Renderer( lua_State* p_L )
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
}

LuaClass_Renderer::~LuaClass_Renderer( void )
{
}

int LuaClass_Renderer::LUA_getinfos( lua_State* p_L )
{
    dsstring descr;
    m_renderer->GetDescr( descr );

    DrawSpace::Interface::Renderer::Characteristics characteristics;
    m_renderer->GetRenderCharacteristics( characteristics );

    lua_pushstring( p_L, descr.c_str() );
    lua_pushinteger( p_L, characteristics.width_resol );
    lua_pushinteger( p_L, characteristics.height_resol );
    lua_pushinteger( p_L, characteristics.fullscreen );
    lua_pushnumber( p_L, characteristics.width_viewport );
    lua_pushnumber( p_L, characteristics.height_viewport );

    return 6;
}
