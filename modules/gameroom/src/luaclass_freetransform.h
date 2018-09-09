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

#ifndef _LUACLASS_FREETRANSFORMASPECTIMPL_H_
#define _LUACLASS_FREETRANSFORMASPECTIMPL_H_

#include "luna.h"
#include "transformaspectimpl.h"
#include "transformaspect.h"

class LuaClass_FreeTransform
{
private:

    DrawSpace::Interface::AspectImplementations::TransformAspectImpl*   m_free_transformer;
    DrawSpace::Aspect::TransformAspect*                                 m_entity_transform_aspect;

public:

	LuaClass_FreeTransform( lua_State* p_L );
	~LuaClass_FreeTransform( void );

    int LUA_instanciateTransformationImpl(lua_State* p_L);
    int LUA_trashTransformationImpl(lua_State* p_L);

    int LUA_configure( lua_State* p_L );
    int LUA_release( lua_State* p_L );
    int LUA_update( lua_State* p_L );
    int LUA_read( lua_State* p_L );

    static const char className[];
    static const Luna<LuaClass_FreeTransform>::RegType methods[];

};

#endif