/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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
/* -*-LIC_END-*- */

#ifndef _LUACLASS_RENDERCONFIG_H_
#define _LUACLASS_RENDERCONFIG_H_

#include "luna.h"
#include "luaclass_rendercontext.h"


class LuaClass_RenderConfig
{
protected:

    std::vector<LuaClass_RenderContext::Data>     m_renderContexts;

public:
	LuaClass_RenderConfig( lua_State* p_L );
	~LuaClass_RenderConfig( void );

    int LUA_addrendercontext( lua_State* p_L );

    int GetRenderContextListSize( void ) const;
    LuaClass_RenderContext::Data GetRenderContext( int p_index ) const;

    static const char className[];
    static const Luna<LuaClass_RenderConfig>::RegType methods[];
};

#endif