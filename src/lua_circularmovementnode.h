/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _LUA_CIRCULARMOVEMENT_H_
#define _LUA_CIRCULARMOVEMENT_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"
#include "circularmovement.h"
#include "luna.h"

class LuaCircularMovementNode
{
public:
    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

protected:

    DrawSpace::Core::SceneNode<DrawSpace::Core::CircularMovement>           m_circular_node;
    DrawSpace::Core::SceneNode<DrawSpace::Core::CircularMovement>*          m_existing_circular_node;

    DrawSpace::Utils::Vector                                                m_centerpos;
    DrawSpace::Utils::Vector                                                m_deltacenterpos;
    DrawSpace::Utils::Vector                                                m_axis;
    dsreal                                                                  m_initial_theta;
    dsreal                                                                  m_initial_phi;
    dsreal                                                                  m_initial_angle;

public:

    LuaCircularMovementNode( lua_State* p_L );
    ~LuaCircularMovementNode( void );

    int Lua_LinkTo( lua_State* p_L );
    
    int Lua_SetCenterpos( lua_State* p_L );
    int Lua_SetDeltaCenterpos( lua_State* p_L );
    int Lua_SetAxis( lua_State* p_L );

    int Lua_SetInitialTheta( lua_State* p_L );
    int Lua_SetInitialPhi( lua_State* p_L );
    int Lua_SetInitialAngle( lua_State* p_L );

    int Lua_SetTheta( lua_State* p_L );
    int Lua_SetPhi( lua_State* p_L );
    int Lua_SetAngularSpeed( lua_State* p_L );


    static const char className[];
    static const Luna2<LuaCircularMovementNode>::RegType methods[];

};

#endif
