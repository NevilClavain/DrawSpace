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

#include "lua_shaderscontroller.h"
#include "luacontext.h"
#include "lua_vector.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

const char LuaShadersController::className[] = "ShadersController";
const Luna2<LuaShadersController>::RegType LuaShadersController::methods[] =
{
    { "Update", &LuaShadersController::Lua_Update },
    { "Print", &LuaShadersController::Lua_Print },
    { 0, 0 }
};

LuaShadersController::LuaShadersController( lua_State* p_L )
{    
    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();
}

LuaShadersController::~LuaShadersController( void ) 
{
}

int LuaShadersController::Lua_Update( lua_State* p_L )
{
    int argc = lua_gettop( p_L );
	if( 2 == argc )
	{
        const char* id = luaL_checkstring( p_L, 1 );
        LuaVector* value = Luna2<LuaVector>::check( p_L, 2 );

        ShadersController::GetInstance()->Update( id, value->m_vector );

	}
    else if( 5 == argc )
    {
        const char* id = luaL_checkstring( p_L, 1 );

        dsreal valx = luaL_checknumber( p_L, 2 );
        dsreal valy = luaL_checknumber( p_L, 3 );
        dsreal valz = luaL_checknumber( p_L, 4 );
        dsreal valw = luaL_checknumber( p_L, 5 );

        ShadersController::GetInstance()->Update( id, Vector( valx, valy, valz, valw ) );
    }

    else
    {
		lua_pushstring( p_L, "UpdateShaderParam : bad number of args" );
		lua_error( p_L );
    }

    return 0;
}

int LuaShadersController::Lua_Print( lua_State* p_L )
{

    return 0;
}

int LuaShadersController::Lua_RegisterFromChunk( lua_State* p_L )
{
    return 0;
}

int LuaShadersController::Lua_RegisterFromIntermediatePass( lua_State* p_L )
{
    return 0;
}
