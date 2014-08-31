/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "lua_matrix.h"


using namespace DrawSpace;
using namespace DrawSpace::Utils;

const char LuaMatrix::className[] = "Matrix";
const DrawSpace::Luna<LuaMatrix>::RegType LuaMatrix::Register[] =
{
    { "GetObject", &LuaMatrix::Lua_GetObject },
    { 0 }
};

LuaMatrix::LuaMatrix( lua_State* p_L )
{
}

LuaMatrix::~LuaMatrix( void ) 
{
}

int LuaMatrix::Lua_GetObject( lua_State* p_L )
{
    lua_pushunsigned( p_L, (lua_Unsigned)&m_matrix );
    return 1;
}
