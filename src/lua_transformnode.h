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

#ifndef _LUA_TRANSFORMNODE_H_
#define _LUA_TRANSFORMNODE_H_

#include "lua_binding.h"
#include "transformnode.h"

namespace DrawSpace
{
class LuaTransformNode : public LuaBinding
{
protected:

    DrawSpace::Core::TransformNode*       m_transformnode;

public:
    LuaTransformNode( lua_State* p_L );
    ~LuaTransformNode( void );

    int Lua_SetObject( lua_State* p_L );

    int Lua_SetName( lua_State* p_L );
    int Lua_GetName( lua_State* p_L );
    int Lua_SetLocalTransform( lua_State* p_L );

    static const char className[];
    static const DrawSpace::Luna<LuaTransformNode>::RegType Register[];
};
}


#endif