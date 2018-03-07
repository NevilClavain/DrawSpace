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

#include "luaclass_renderpassnodegraph.h"

const char LuaClass_RenderPassNodeGraph::className[] = "RenderPassNodeGraph";
const Luna<LuaClass_RenderPassNodeGraph>::RegType LuaClass_RenderPassNodeGraph::methods[] =
{
	{ 0, 0 }
};

LuaClass_RenderPassNodeGraph::LuaClass_RenderPassNodeGraph( lua_State* p_L )
{
}

LuaClass_RenderPassNodeGraph::~LuaClass_RenderPassNodeGraph( void )
{
}