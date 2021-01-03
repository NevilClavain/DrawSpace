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

#ifndef _LUACLASS_FPSRAWTRANSFOR_H_
#define _LUACLASS_FPSRAWTRANSFOR_H_

#include "luna.h"
#include "rawtransformaspectimpl.h"
#include "transformaspect.h"


class LuaClass_RawTransform
{
private:
    DrawSpace::AspectImplementations::RawTransformAspectImpl    m_raw_transformer;
    DrawSpace::Aspect::TransformAspect*                         m_entity_transform_aspect;
    std::vector<std::string>                                    m_matrix_ids;  // les ids de tt les composants Matrix

public:

	LuaClass_RawTransform( lua_State* p_L );
	~LuaClass_RawTransform( void );

    int LUA_configure( lua_State* p_L );
    int LUA_release( lua_State* p_L );
    int LUA_addmatrix( lua_State* p_L );
    int LUA_updatematrix( lua_State* p_L );

    static const char className[];
    static const Luna<LuaClass_RawTransform>::RegType methods[];

};

#endif
