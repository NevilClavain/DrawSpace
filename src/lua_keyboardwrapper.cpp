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

#include "lua_keyboardwrapper.h"
#include "luacontext.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaKeyboardWrapper::className[] = "KeyboardWrapper";
const Luna2<LuaKeyboardWrapper>::RegType LuaKeyboardWrapper::methods[] =
{
    { "GetLastKeyupCode", &LuaKeyboardWrapper::Lua_GetLastKeyupCode },
    { "GetLastKeydownCode", &LuaKeyboardWrapper::Lua_GetLastKeydownCode },
    { 0, 0 }
};

LuaKeyboardWrapper::LuaKeyboardWrapper( lua_State* p_L )
{    
    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "KeyboardWrapper:KeyboardWrapper" );
        props.AddPropValue<int*>( "keyupcode", &m_keyupcode );
        props.AddPropValue<int*>( "keydowncode", &m_keydowncode );

        (*m_scriptcalls_handler)( props );
    }

}

LuaKeyboardWrapper::~LuaKeyboardWrapper( void ) 
{
}

int LuaKeyboardWrapper::Lua_GetLastKeyupCode( lua_State* p_L )
{
    lua_pushinteger( p_L, m_keyupcode );
    return 1;
}

int LuaKeyboardWrapper::Lua_GetLastKeydownCode( lua_State* p_L )
{
    lua_pushinteger( p_L, m_keydowncode );
    return 1;
}