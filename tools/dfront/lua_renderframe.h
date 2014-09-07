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

#ifndef _LUA_RENDERFRAME_H_
#define _LUA_RENDERFRAME_H_

#include "luna.h"
#include "renderframe.h"

class LuaRenderFrame
{
protected:

    RenderFrame* m_instance;

public:
    LuaRenderFrame( lua_State* p_L );
    ~LuaRenderFrame( void );
  
    int Lua_GetScene( lua_State* p_L );
    int Lua_InstanciateScene( lua_State* p_L );
    int Lua_SetCurrentScene( lua_State* p_L );

    int Lua_GetMesheImportObject( lua_State* p_L );

    static const char className[];
    static const Luna<LuaRenderFrame>::RegType Register[];
};

#endif
