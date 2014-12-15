
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

#ifndef _LUA_DRAWSPACE_H_
#define _LUA_DRAWSPACE_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "luna.h"

class LuaDrawSpace
{
public:

    DrawSpace::Core::BaseCallback<void, bool>*       m_displayframerate_handler;

protected:


public:
    LuaDrawSpace( lua_State* p_L );
    ~LuaDrawSpace( void );
     
    int Lua_DisplayFramerate( lua_State* p_L );

    static const char className[];
    static const Luna<LuaDrawSpace>::RegType Register[];
};

#endif