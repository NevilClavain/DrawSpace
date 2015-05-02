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

#ifndef _LUA_CAMERAPOINTNODE_H_
#define _LUA_CAMERAPOINTNODE_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"
#include "camerapoint.h"
#include "luna.h"

class LuaCameraPointNode
{
public:

    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

public:

    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>             m_camera_node;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*            m_existing_camera_node;
    

public:

    LuaCameraPointNode( lua_State* p_L );
    ~LuaCameraPointNode( void );
     
    int Lua_LinkTo( lua_State* p_L );
    int Lua_LoadScript( lua_State* p_L );

    int Lua_LockOn( lua_State* p_L );
    int Lua_Unlock( lua_State* p_L );
    int Lua_UpdateProjectionZNear( lua_State* p_L );

    static const char className[];
    static const Luna2<LuaCameraPointNode>::RegType methods[];
};

#endif