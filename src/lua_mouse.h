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

#ifndef _LUA_MOUSE_H_
#define _LUA_MOUSE_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"
#include "luna.h"
#include "descriptors.h"

class LuaMouse
{
public:

    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

protected:

    DrawSpace::Utils::MouseMovementsDescriptor  m_mouse_movements;

public:

    LuaMouse( lua_State* p_L );
    ~LuaMouse( void );

    int Lua_GetLastXMouse( lua_State* p_L );
    int Lua_GetLastYMouse( lua_State* p_L );

    int Lua_GetLastDeltaXMouse( lua_State* p_L );
    int Lua_GetLastDeltaYMouse( lua_State* p_L );

    int Lua_IsLeftButtonDown( lua_State* p_L );
    int Lua_IsRightButtonDown( lua_State* p_L );


    static const char className[];
    static const Luna2<LuaMouse>::RegType methods[];

};

#endif
