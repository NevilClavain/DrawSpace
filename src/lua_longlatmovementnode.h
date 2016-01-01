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

#ifndef _LUA_LONGLATMOVEMENT_H_
#define _LUA_LONGLATMOVEMENT_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"
#include "longlatmovement.h"
#include "luna.h"


class LuaLongLatMovementNode
{
public:
    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

protected:

    DrawSpace::Core::SceneNode<DrawSpace::Core::LongLatMovement>            m_longlat_node;
    DrawSpace::Core::SceneNode<DrawSpace::Core::LongLatMovement>*           m_existing_longlat_node;

    dsreal                                                                  m_initial_theta;
    dsreal                                                                  m_initial_phi;
    dsreal                                                                  m_initial_alt;
    dsreal                                                                  m_initial_long;
    dsreal                                                                  m_initial_lat;


public:

    LuaLongLatMovementNode( lua_State* p_L );
    ~LuaLongLatMovementNode( void );

    int Lua_LinkTo( lua_State* p_L );
    
    int Lua_SetInitialTheta( lua_State* p_L );
    int Lua_SetInitialPhi( lua_State* p_L );
    int Lua_SetInitialAlt( lua_State* p_L );
    int Lua_SetInitialLongitud( lua_State* p_L );
    int Lua_SetInitialLatitud( lua_State* p_L );


    int Lua_SetTheta( lua_State* p_L );
    int Lua_SetPhi( lua_State* p_L );
    int Lua_SetAlt( lua_State* p_L );
    int Lua_SetLongitud( lua_State* p_L );
    int Lua_SetLatitud( lua_State* p_L );


    static const char className[];
    static const Luna2<LuaLongLatMovementNode>::RegType methods[];

};


#endif
