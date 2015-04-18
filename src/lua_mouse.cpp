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

#include "lua_mouse.h"
#include "luacontext.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

const char LuaMouse::className[] = "Mouse";
const Luna2<LuaMouse>::RegType LuaMouse::methods[] =
{
    { "GetLastXMouse", &LuaMouse::Lua_GetLastXMouse },
    { "GetLastYMouse", &LuaMouse::Lua_GetLastYMouse },
    { "GetLastDeltaXMouse", &LuaMouse::Lua_GetLastDeltaXMouse },
    { "GetLastDeltaYMouse", &LuaMouse::Lua_GetLastDeltaYMouse },
    { "IsLeftButtonDown", &LuaMouse::Lua_IsLeftButtonDown },
    { "IsRightButtonDown", &LuaMouse::Lua_IsRightButtonDown },
    { 0, 0 }
};

LuaMouse::LuaMouse( lua_State* p_L )
{    
    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();

    m_mouse_movements.delta_xmouse = 0;
    m_mouse_movements.delta_ymouse = 0;
    m_mouse_movements.xmouse = 0;
    m_mouse_movements.ymouse = 0;
    m_mouse_movements.leftbutton_down = false;
    m_mouse_movements.rightbutton_down = false;

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "Mouse:Mouse" );
        props.AddPropValue<MouseMovementsDescriptor*>( "descriptor", &m_mouse_movements );

        (*m_scriptcalls_handler)( props );
    }

}

LuaMouse::~LuaMouse( void ) 
{
}

int LuaMouse::Lua_GetLastXMouse( lua_State* p_L )
{
    lua_pushinteger( p_L, m_mouse_movements.xmouse );
    return 1;
}

int LuaMouse::Lua_GetLastYMouse( lua_State* p_L )
{
    lua_pushinteger( p_L, m_mouse_movements.ymouse );
    return 1;
}

int LuaMouse::Lua_GetLastDeltaXMouse( lua_State* p_L )
{
    lua_pushinteger( p_L, m_mouse_movements.delta_xmouse );
    return 1;
}

int LuaMouse::Lua_GetLastDeltaYMouse( lua_State* p_L )
{
    lua_pushinteger( p_L, m_mouse_movements.delta_ymouse );
    return 1;
}

int LuaMouse::Lua_IsLeftButtonDown( lua_State* p_L )
{
    lua_pushinteger( p_L, m_mouse_movements.leftbutton_down );
    return 1;
}

int LuaMouse::Lua_IsRightButtonDown( lua_State* p_L )
{
    lua_pushinteger( p_L, m_mouse_movements.rightbutton_down );
    return 1;
}
