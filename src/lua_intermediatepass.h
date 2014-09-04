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

#ifndef _LUA_INTERMEDIATEPASS_H_
#define _LUA_INTERMEDIATEPASS_H_

#include "lua_binding.h"
#include "pass.h"

namespace DrawSpace
{
class LuaIntermediatePass : public LuaBinding
{
protected:

    bool                                m_release_object;
    DrawSpace::IntermediatePass*        m_intermediatepass;

    void                                cleanup( void );

public:
    LuaIntermediatePass( lua_State* p_L );
    ~LuaIntermediatePass( void );

    DrawSpace::IntermediatePass* GetObject( void ) { return m_intermediatepass; };

    int Lua_SetObject( lua_State* p_L );
    int Lua_GetObject( lua_State* p_L );

    int Lua_InstanciateObject( lua_State* p_L );


    static const char className[];
    static const DrawSpace::Luna<LuaIntermediatePass>::RegType Register[];
};
}


#endif