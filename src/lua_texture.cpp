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

#include "lua_texture.h"
#include "luacontext.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaTexture::className[] = "Texture";
const Luna2<LuaTexture>::RegType LuaTexture::methods[] =
{  
    { 0 }
};

LuaTexture::LuaTexture( lua_State* p_L ) :
m_texture( NULL )
{
	int argc = lua_gettop( p_L );

    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();    
}

LuaTexture::~LuaTexture( void ) 
{
}