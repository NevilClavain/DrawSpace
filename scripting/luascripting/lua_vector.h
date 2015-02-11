/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#ifndef _LUA_VECTOR_H_
#define _LUA_VECTOR_H_

#include "vector.h"
#include "luna.h"

class LuaVector
{
public:

    DrawSpace::Utils::Vector m_vector;

public:

    LuaVector( lua_State* p_L );
    ~LuaVector( void );

    int Lua_Set( lua_State* p_L );
    int Lua_Get( lua_State* p_L );
    int Lua_Lenght( lua_State* p_L );
    int Lua_LenghtPow2( lua_State* p_L );
    int Lua_Normalize( lua_State* p_L );
    int Lua_Scale( lua_State* p_L );

    static const char className[];
    static const Luna2<LuaVector>::RegType methods[];

};

#endif