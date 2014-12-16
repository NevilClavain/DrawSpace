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

#ifndef _LUA_SPACEBOXBUILDER_H_
#define _LUA_SPACEBOXBUILDER_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"
#include "spacebox.h"
#include "luna.h"

class LuaSpaceboxBuilder
{
public:

    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

protected:

    DrawSpace::Spacebox::Descriptor m_descriptor;



public:
    LuaSpaceboxBuilder( lua_State* p_L );
    ~LuaSpaceboxBuilder( void );
     
    int Lua_SetSceneName( lua_State* p_L );
    int Lua_RegisterPassSlot( lua_State* p_L );
    int Lua_SetPassSlotFxName( lua_State* p_L );
    int Lua_SetPassSlotRenderingOrder( lua_State* p_L );
    int Lua_SetPassSlotTextureName( lua_State* p_L );
    int Lua_BuildIt( lua_State* p_L );

    
    static const char className[];
    static const Luna<LuaSpaceboxBuilder>::RegType Register[];
};

#endif