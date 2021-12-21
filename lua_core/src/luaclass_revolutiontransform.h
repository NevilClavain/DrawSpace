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

#pragma once

#include "luna.h"
#include "revolutiontransformaspectimpl.h"
#include "transformaspect.h"


class LuaClass_RevolutionTransform
{
private:
    DrawSpace::AspectImplementations::RevolutionTransformAspectImpl     m_revolution_transformer;
    DrawSpace::Aspect::TransformAspect*                                 m_entity_transform_aspect;
    

public:

    LuaClass_RevolutionTransform( lua_State* p_L );
	~LuaClass_RevolutionTransform( void );

    int LUA_configure( lua_State* p_L );
    int LUA_release( lua_State* p_L );

    int LUA_readcurrentangle(lua_State* p_L);


    static const char className[];
    static const Luna<LuaClass_RevolutionTransform>::RegType methods[];

};

