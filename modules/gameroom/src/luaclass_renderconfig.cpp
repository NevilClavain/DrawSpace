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

#include "luacontext.h"
#include "luaclass_renderconfig.h"
#include "luaclass_rendercontext.h"

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
    LuaClass_RenderConfig* lua_rc = Luna<LuaClass_RenderConfig>::check( p_L, 1 );
    if( NULL == lua_rc )
    {
        LUA_ERROR( "RenderConfig::add_rendercontext : argument 1 must be of type LuaClass_RenderConfig" );
    }
    return 0;
}

int LuaClass_RenderConfig::GetRenderContextListSize( void ) const
{
    return m_renderContexts.size();
}

LuaClass_RenderContext* LuaClass_RenderConfig::GetRenderContext( int p_index ) const
{
    return m_renderContexts[p_index];
}
