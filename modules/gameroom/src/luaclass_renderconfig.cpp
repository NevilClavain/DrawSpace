/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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
#include "luaclass_renderconfig.h"


const char LuaClass_RenderConfig::className[] = "RenderConfig";
const Luna<LuaClass_RenderConfig>::RegType LuaClass_RenderConfig::methods[] =
{
    { "add_rendercontext", &LuaClass_RenderConfig::LUA_addrendercontext },
	{ 0, 0 }
};

LuaClass_RenderConfig::LuaClass_RenderConfig( lua_State* p_L )
{
}

LuaClass_RenderConfig::~LuaClass_RenderConfig( void )
{
}

int LuaClass_RenderConfig::LUA_addrendercontext( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "RenderConfig::add_rendercontext : argument(s) missing" );
	}
    LuaClass_RenderContext* lua_rc = Luna<LuaClass_RenderContext>::check( p_L, 1 );

    m_data.render_contexts.push_back( lua_rc->GetData() );
    return 0;
}
