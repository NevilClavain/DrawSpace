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

#include "meshluaext.h"
#include "luaclass_meshrendering.h"
#include "luaclass_linemeshrendering.h"
#include "plugin.h"
#include "memalloc.h"

MeshLuaExtension::MeshLuaExtension(void)
{
}

void MeshLuaExtension::Register(lua_State* p_L)
{
	Luna<LuaClass_MeshRendering>::Register(p_L);
	Luna<LuaClass_LineMeshRendering>::Register(p_L);
}

void MeshLuaExtension::UpdateRenderer(DrawSpace::Interface::Renderer* p_renderer)
{
	DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface = p_renderer;
}

DrawSpace::Utils::MemAlloc* MeshLuaExtension::GetMemAllocInstance(void) const
{
	return DrawSpace::Utils::MemAlloc::GetInstance();
}
