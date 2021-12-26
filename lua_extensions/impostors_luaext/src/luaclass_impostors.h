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

#ifndef _LUACLASS_RENDERER_H_
#define _LUACLASS_RENDERER_H_
#include "luna.h"

// fwd decls
namespace DrawSpace
{
namespace Core
{
class Entity;
}
namespace Aspect
{
class RenderingAspect;
}
};

class ImpostorsRenderingAspectImpl;
// fwd decls

class LuaClass_Impostors
{
private:

    ImpostorsRenderingAspectImpl*           m_impostors_render;
    DrawSpace::Aspect::RenderingAspect*     m_entity_rendering_aspect;
    DrawSpace::Core::Entity*                m_entity;


public:
    LuaClass_Impostors( lua_State* p_L );
	~LuaClass_Impostors( void );

    int LUA_attachtoentity(lua_State* p_L);
    int LUA_detachfromentity(lua_State* p_L);

    static const char className[];
    static const Luna<LuaClass_Impostors>::RegType methods[];

};

#endif
