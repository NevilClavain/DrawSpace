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

#include "lua_mousewrapper.h"
#include "luacontext.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

const char LuaMouseWrapper::className[] = "MouseWrapper";
const Luna2<LuaMouseWrapper>::RegType LuaMouseWrapper::methods[] =
{
    { "GetLastXMouse", &LuaMouseWrapper::Lua_GetLastXMouse },
    { "GetLastYMouse", &LuaMouseWrapper::Lua_GetLastYMouse },
    { "GetLastDeltaXMouse", &LuaMouseWrapper::Lua_GetLastDeltaXMouse },
    { "GetLastDeltaYMouse", &LuaMouseWrapper::Lua_GetLastDeltaYMouse },
    { "IsLeftButtonDown", &LuaMouseWrapper::Lua_IsLeftButtonDown },
    { "IsRightButtonDown", &LuaMouseWrapper::Lua_IsRightButtonDown },


    { 0, 0 }
};

LuaMouseWrapper::LuaMouseWrapper( lua_State* p_L )
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
        props.AddPropValue<dsstring>( "script_call_id", "MouseWrapper:MouseWrapper" );
        props.AddPropValue<MouseMovementsDescriptor*>( "descriptor", &m_mouse_movements );

        (*m_scriptcalls_handler)( props );
    }

}

LuaMouseWrapper::~LuaMouseWrapper( void ) 
{
}

int LuaMouseWrapper::Lua_GetLastXMouse( lua_State* p_L )
{
    lua_pushinteger( p_L, m_mouse_movements.xmouse );
    return 1;
}

int LuaMouseWrapper::Lua_GetLastYMouse( lua_State* p_L )
{
    lua_pushinteger( p_L, m_mouse_movements.ymouse );
    return 1;
}

int LuaMouseWrapper::Lua_GetLastDeltaXMouse( lua_State* p_L )
{
    lua_pushinteger( p_L, m_mouse_movements.delta_xmouse );
    return 1;
}

int LuaMouseWrapper::Lua_GetLastDeltaYMouse( lua_State* p_L )
{
    lua_pushinteger( p_L, m_mouse_movements.delta_ymouse );
    return 1;
}

int LuaMouseWrapper::Lua_IsLeftButtonDown( lua_State* p_L )
{
    lua_pushinteger( p_L, m_mouse_movements.leftbutton_down );
    return 1;
}

int LuaMouseWrapper::Lua_IsRightButtonDown( lua_State* p_L )
{
    lua_pushinteger( p_L, m_mouse_movements.rightbutton_down );
    return 1;
}
