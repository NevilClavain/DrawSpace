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

#ifndef _LUA_MOVEMENTBUILDER_H_
#define _LUA_MOVEMENTBUILDER_H_

#include "drawspace_commons.h"
#include "vector.h"
#include "callback.h"
#include "mediator.h"
#include "luna.h"

class LuaMovementBuilder
{
public:

    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

protected:

    dsstring                    m_name;
    dsstring                    m_type;
    DrawSpace::Utils::Vector    m_initpos;
    DrawSpace::Utils::Vector    m_direction;
    DrawSpace::Utils::Vector    m_centerpos;
    DrawSpace::Utils::Vector    m_rotationaxis;
    dsreal                      m_initialangle;
    dsreal                      m_scalefactor;
    dsreal                      m_refforce;
    long                        m_period;
    bool                        m_attach_to_orbiter;

    dsreal                      m_initial_theta;
    dsreal                      m_initial_phi;
    dsreal                      m_initial_long;
    dsreal                      m_initial_lat;
    dsreal                      m_initial_alt;


public:
    LuaMovementBuilder( lua_State* p_L );
    ~LuaMovementBuilder( void );
     
    int Lua_SetName( lua_State* p_L );
    int Lua_SetType( lua_State* p_L );
    int Lua_SetInitpos( lua_State* p_L );
    int Lua_SetDirection( lua_State* p_L );
    int Lua_SetInitialTheta( lua_State* p_L );
    int Lua_SetInitialPhi( lua_State* p_L );
    int Lua_SetInitialLong( lua_State* p_L );
    int Lua_SetInitialLat( lua_State* p_L );
    int Lua_SetInitialAlt( lua_State* p_L );
    int Lua_SetCenterPos( lua_State* p_L );
    int Lua_SetDeltaCenter( lua_State* p_L );
    int Lua_SetRotationAxis( lua_State* p_L );
    int Lua_SetInitialAngle( lua_State* p_L );
    int Lua_SetScaleFactor( lua_State* p_L );
    int Lua_SetRefForce( lua_State* p_L );
    int Lua_SetPeriod( lua_State* p_L );
    int Lua_AttachToOrbiter( lua_State* p_L );
    int Lua_BuildIt( lua_State* p_L );

    
    static const char className[];
    static const Luna<LuaMovementBuilder>::RegType Register[];
};

#endif