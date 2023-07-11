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
#include "renderingaspectimpl.h"
#include "luaclass_entity.h"
#include "renderingaspect.h"

namespace DrawSpace
{
namespace Utils
{
class TimeManager;
}
}

// rendering implementation from a module, encapsulated in a lua class
class LuaClass_Rendering
{
private:

    DrawSpace::Interface::AspectImplementations::RenderingAspectImpl*   m_rendering_impl;
    DrawSpace::Aspect::RenderingAspect*                                 m_entity_rendering_aspect;
    DrawSpace::Core::Entity*                                            m_entity;

	// table de traduction RenderContext name -> Passes Name
	// permet de savoir a quelle passe est attribu�e un rendercontext

    std::map<dsstring, std::vector<dsstring>>						    m_rcname_to_passes;

    size_t                                                              m_total_mem_allocs;
    DrawSpace::Utils::TimeManager*                                      m_tm;

    void cleanup_resources( lua_State* p_L );

public:
    LuaClass_Rendering( lua_State* p_L );
	~LuaClass_Rendering( void );

    int LUA_instanciateRenderingImpl( lua_State* p_L );
    int LUA_trashRenderingImpl( lua_State* p_L );

    int LUA_attachtoentity( lua_State* p_L );
    int LUA_detachfromentity( lua_State* p_L );

    int LUA_configure( lua_State* p_L );
    int LUA_release( lua_State* p_L );

    int LUA_registertorendering( lua_State* p_L );
    int LUA_unregisterfromrendering( lua_State* p_L );

    //int LUA_setshaderrealvector( lua_State* p_L );

	int LUA_setPassForRenderId(lua_State* p_L);

    inline DrawSpace::Aspect::RenderingAspect* GetRenderingAspect(void) const 
    { 
        return m_entity_rendering_aspect;
    }

    static const char className[];
    static const Luna<LuaClass_Rendering>::RegType methods[];
};

