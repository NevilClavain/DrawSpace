/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#pragma once

/*
#include "drawspace_commons.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};
*/

//fwd decl
struct lua_State;
namespace DrawSpace
{
namespace Interface
{
class Renderer;
}
}
class LuaExtension
{
public:
	virtual void Register(lua_State*) = 0;
	virtual void UpdateRenderer(DrawSpace::Interface::Renderer*) = 0;
};
