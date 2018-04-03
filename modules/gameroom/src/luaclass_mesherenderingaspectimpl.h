/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

#ifndef _LUACLASS_MESHERENDERINGASPECTIMPL_H_
#define _LUACLASS_MESHERENDERINGASPECTIMPL_H_

#include "luna.h"
#include "mesherenderingaspectimpl.h"
#include "luaclass_entity.h"
#include "renderingaspect.h"
#include "mesheimport.h"

class LuaClass_MesheRenderingAspectImpl
{
private:
    DrawSpace::Core::Meshe                                       m_meshe;
    DrawSpace::Core::Fx                                          m_fx;
    DrawSpace::AspectImplementations::MesheRenderingAspectImpl*  m_meshe_render;
    DrawSpace::Aspect::RenderingAspect*                          m_entity_rendering_aspect;
    dsstring                                                     m_id;

    void cleanup_resources( lua_State* p_L );

public:

	LuaClass_MesheRenderingAspectImpl( lua_State* p_L );
	~LuaClass_MesheRenderingAspectImpl( void );

    int LUA_configure( lua_State* p_L );
    int LUA_release( lua_State* p_L );
    int LUA_registertorendering( lua_State* p_L );
    int LUA_unregisterfromrendering( lua_State* p_L );
   
    
    static const char className[];
    static const Luna<LuaClass_MesheRenderingAspectImpl>::RegType methods[];
    
};

#endif