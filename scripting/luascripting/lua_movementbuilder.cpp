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

#include "lua_movementbuilder.h"
#include "luacontext.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaMovementBuilder::className[] = "MovementBuilder";
const Luna<LuaMovementBuilder>::RegType LuaMovementBuilder::Register[] =
{
  { "SetName", &LuaMovementBuilder::Lua_SetName },
  { "SetType", &LuaMovementBuilder::Lua_SetType },
  { "SetInitpos", &LuaMovementBuilder::Lua_SetInitpos },
  { "SetDirection", &LuaMovementBuilder::Lua_SetDirection },
  { "SetCenterPos", &LuaMovementBuilder::Lua_SetCenterPos },
  { "SetDeltaCenter", &LuaMovementBuilder::Lua_SetDeltaCenter },
  { "SetRotationAxis", &LuaMovementBuilder::Lua_SetRotationAxis },
  { "SetInitialAngle", &LuaMovementBuilder::Lua_SetInitialAngle },
  { "SetInitialTheta", &LuaMovementBuilder::Lua_SetInitialTheta },
  { "SetInitialPhi", &LuaMovementBuilder::Lua_SetInitialPhi },
  { "SetScaleFactor", &LuaMovementBuilder::Lua_SetScaleFactor },
  { "SetRefForce", &LuaMovementBuilder::Lua_SetRefForce },
  { "SetPeriod", &LuaMovementBuilder::Lua_SetPeriod },
  { "AttachToOrbiter", &LuaMovementBuilder::Lua_AttachToOrbiter },
  { "SetInitialLong", &LuaMovementBuilder::Lua_SetInitialLong },
  { "SetInitialLat", &LuaMovementBuilder::Lua_SetInitialLat },
  { "SetInitialAlt", &LuaMovementBuilder::Lua_SetInitialAlt },
  { "BuildIt", &LuaMovementBuilder::Lua_BuildIt },
  { 0 }
};


LuaMovementBuilder::LuaMovementBuilder( lua_State* p_L ) :
m_initialangle( 0.0 ),
m_scalefactor( 1.0 ),
m_refforce( 1.0 ),
m_period( 10 ),
m_attach_to_orbiter( false ),
m_initial_theta( 0.0 ),
m_initial_phi( 0.0 ),
m_initial_long( 0.0 ),
m_initial_lat( 0.0 ),
m_initial_alt( 1.0 )
{   
    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();
}

LuaMovementBuilder::~LuaMovementBuilder( void ) 
{
}

int LuaMovementBuilder::Lua_SetName( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetName : bad number of args" );
		lua_error( p_L );		
	}
    const char* name = luaL_checkstring( p_L, 2 );
    m_name = name;

    return 0;
}

int LuaMovementBuilder::Lua_SetType( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetName : bad number of args" );
		lua_error( p_L );		
	}
    const char* type = luaL_checkstring( p_L, 2 );
    m_type = type;

    return 0;
}

int LuaMovementBuilder::Lua_SetInitpos( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 4 )
	{
		lua_pushstring( p_L, "SetInitpos : bad number of args" );
		lua_error( p_L );		
	}

    m_initpos[0] = luaL_checknumber( p_L, 2 );
    m_initpos[1] = luaL_checknumber( p_L, 3 );
    m_initpos[2] = luaL_checknumber( p_L, 4 );
    m_initpos[3] = 1.0;

    return 0;
}

int LuaMovementBuilder::Lua_SetDirection( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_SetInitialTheta( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_SetInitialPhi( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_SetCenterPos( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_SetDeltaCenter( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_SetRotationAxis( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_SetInitialAngle( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_SetScaleFactor( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_SetRefForce( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_SetPeriod( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_AttachToOrbiter( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_SetInitialLong( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_SetInitialLat( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_SetInitialAlt( lua_State* p_L )
{
    return 0;
}

int LuaMovementBuilder::Lua_BuildIt( lua_State* p_L )
{
    return 0;
}