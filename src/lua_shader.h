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

#ifndef _LUA_SHADER_H_
#define _LUA_SHADER_H_

#include "luna.h"
#include "shader.h"

namespace DrawSpace
{
class LuaShader
{
protected:

    bool                        m_release_object;
    DrawSpace::Core::Shader*    m_shader;

    void                        cleanup( void );

public:
    LuaShader( lua_State* p_L );
    ~LuaShader( void );

    int Lua_SetObject( lua_State* p_L );
    int Lua_GetObject( lua_State* p_L );

    int Lua_InstanciateObject( lua_State* p_L );
    int Lua_LoadFromFile( lua_State* p_L );

    static const char className[];
    static const DrawSpace::Luna<LuaShader>::RegType Register[];
};
}

#endif