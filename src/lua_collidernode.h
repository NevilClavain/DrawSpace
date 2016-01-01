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

#ifndef _LUA_COLLIDERNODE_H_
#define _LUA_COLLIDERNODE_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"
#include "collider.h"

#include "luna.h"

class LuaColliderNode
{
public:

    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Collider>               m_collider_node;
    DrawSpace::Dynamics::Body::Parameters                                   m_params;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Collider>*              m_existing_collider_node;

    LuaColliderNode( lua_State* p_L );
    ~LuaColliderNode( void );
     
    int Lua_IsValid( lua_State* p_L );
    int Lua_SetShapeDescrSphere( lua_State* p_L );
    int Lua_SetShapeDescrBox( lua_State* p_L );


    int Lua_GetShapeType( lua_State* p_L );
    int Lua_GetShapeDescrBoxDimX( lua_State* p_L );
    int Lua_GetShapeDescrBoxDimY( lua_State* p_L );
    int Lua_GetShapeDescrBoxDimZ( lua_State* p_L );
    int Lua_GetShapeDescrSphereRadius( lua_State* p_L );

    int Lua_LinkTo( lua_State* p_L );

    static const char className[];
    static const Luna2<LuaColliderNode>::RegType methods[];

};


#endif
