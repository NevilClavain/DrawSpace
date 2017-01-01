/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _LUA_SHADERSCONTROLLER_H_
#define _LUA_SHADERSCONTROLLER_H_


#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"
#include "luna.h"
#include "shaderscontroller.h"

class LuaShadersController
{
public:

    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

protected:


public:

    LuaShadersController( lua_State* p_L );
    ~LuaShadersController( void );

    int Lua_Update( lua_State* p_L );
    int Lua_Print( lua_State* p_L );
    int Lua_RegisterFromChunk( lua_State* p_L );
    int Lua_RegisterFromIntermediatePass( lua_State* p_L );


    static const char className[];
    static const Luna2<LuaShadersController>::RegType methods[];

};

#endif
