/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#ifndef _LUA_MATRIX_H_
#define _LUA_MATRIX_H_

#include "matrix.h"
#include "luna.h"

class LuaMatrix
{
public:

    DrawSpace::Utils::Matrix m_mat;

public:

    LuaMatrix( lua_State* p_L );
    ~LuaMatrix( void );

    int Lua_Set( lua_State* p_L );
    int Lua_Get( lua_State* p_L );

    int Lua_Zero( lua_State* p_L );
    int Lua_Identity( lua_State* p_L );

    int Lua_Translation( lua_State* p_L );
    int Lua_Transpose( lua_State* p_L );
    int Lua_Perspective( lua_State* p_L );

    int Lua_Scale( lua_State* p_L );
    int Lua_ClearTranslation( lua_State* p_L );
    int Lua_Rotation( lua_State* p_L );
    int Lua_Inverse( lua_State* p_L );
    int Lua_Transform( lua_State* p_L );

    

    static const char className[];
    static const Luna2<LuaMatrix>::RegType methods[];


};

#endif
