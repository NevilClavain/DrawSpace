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

#ifndef _LUA_FPSMOVEMENT_H_
#define _LUA_FPSMOVEMENT_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"
#include "fpsmovement.h"
#include "luna.h"

class LuaFpsMovementNode
{
public:

    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

protected:

    DrawSpace::Core::SceneNode<DrawSpace::Core::FPSMovement>                m_fps_node;
    DrawSpace::Core::SceneNode<DrawSpace::Core::FPSMovement>*               m_existing_fps_node;

    DrawSpace::Utils::Vector                                                m_initpos;
    dsreal                                                                  m_initial_theta;
    dsreal                                                                  m_initial_phi;
    bool                                                                    m_ymvt;
    

public:

    LuaFpsMovementNode( lua_State* p_L );
    ~LuaFpsMovementNode( void );
     
    int Lua_LinkTo( lua_State* p_L );
    int Lua_LoadScript( lua_State* p_L );

    int Lua_SetInitpos( lua_State* p_L );
    int Lua_SetInitialTheta( lua_State* p_L );
    int Lua_SetInitialPhi( lua_State* p_L );
    int Lua_SetYMvt( lua_State* p_L );


    int Lua_RotateYaw( lua_State* p_L );
    int Lua_RotatePitch( lua_State* p_L );


    static const char className[];
    static const Luna2<LuaFpsMovementNode>::RegType methods[];
};

#endif