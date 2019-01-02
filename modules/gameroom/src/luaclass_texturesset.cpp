/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#include "luacontext.h"
#include "luaclass_texturesset.h"

const char LuaClass_TexturesSet::className[] = "TexturesSet";
const Luna<LuaClass_TexturesSet>::RegType LuaClass_TexturesSet::methods[] =
{
    { "set_texturefiletostage", &LuaClass_TexturesSet::LUA_settexturefiletostage },
	{ 0, 0 }
};

LuaClass_TexturesSet::LuaClass_TexturesSet( lua_State* p_L )
{
}

LuaClass_TexturesSet::~LuaClass_TexturesSet( void )
{
}

int LuaClass_TexturesSet::LUA_settexturefiletostage( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{		
        LUA_ERROR( "TexturesSet::set_texturefiletostage : argument(s) missing" );
	}

    dsstring texture_path = luaL_checkstring( p_L, 1 );
    int stage = luaL_checkint( p_L, 2 );

    m_data.textures[stage] = texture_path;

    return 0;
}
