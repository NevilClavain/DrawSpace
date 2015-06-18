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

#ifndef _LUA_FREEMOVEMENT_H_
#define _LUA_FREEMOVEMENT_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"
#include "freemovement.h"
#include "luna.h"

class LuaFreeMovementNode
{
public:

    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

protected:

    DrawSpace::Core::SceneNode<DrawSpace::Core::FreeMovement>                m_free_node;
    DrawSpace::Core::SceneNode<DrawSpace::Core::FreeMovement>*               m_existing_free_node;

    DrawSpace::Utils::Vector                                                 m_initpos;
    

public:

    LuaFreeMovementNode( lua_State* p_L );
    ~LuaFreeMovementNode( void );
     
    int Lua_LinkTo( lua_State* p_L );    
    int Lua_SetInitpos( lua_State* p_L );

    int Lua_RotateYaw( lua_State* p_L );
    int Lua_RotatePitch( lua_State* p_L );
    int Lua_RotateRoll( lua_State* p_L );
    int Lua_SetSpeed( lua_State* p_L );

    static const char className[];
    static const Luna2<LuaFreeMovementNode>::RegType methods[];
};


#endif
