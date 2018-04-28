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
#include "luaclass_fxparams.h"
#include "luaclass_renderstatesset.h"

const char LuaClass_FxParams::className[] = "FxParams";
const Luna<LuaClass_FxParams>::RegType LuaClass_FxParams::methods[] =
{
    { "set_renderstatesset", &LuaClass_FxParams::LUA_setrenderstatesset },
    { "add_shaderfile", &LuaClass_FxParams::LUA_addshaderfile },
	{ 0, 0 }
};

LuaClass_FxParams::LuaClass_FxParams( lua_State* p_L )
{
}

LuaClass_FxParams::~LuaClass_FxParams( void )
{
}

int LuaClass_FxParams::LUA_setrenderstatesset( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "FxParams::set_renderstatesset : argument(s) missing" );
	}

    LuaClass_RenderStatesSet* lua_rss = Luna<LuaClass_RenderStatesSet>::check( p_L, 1 );

    m_rss = lua_rss->GetRenderStatesSet();
    return 0;
}

int LuaClass_FxParams::LUA_addshaderfile( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{		
        LUA_ERROR( "FxParams::add_shaderfile : argument(s) missing" );
	}

    dsstring shader_path = luaL_checkstring( p_L, 1 );
    bool is_compiled = luaL_checkint( p_L, 2 );

    std::pair<dsstring,bool> shader_info( shader_path, is_compiled );
    m_shaders.push_back( shader_info );
    return 0;
}

DrawSpace::Core::RenderStatesSet LuaClass_FxParams::GetRenderStatesSet( void ) const
{
    return m_rss;
}

size_t LuaClass_FxParams::GetNbShaderFiles( void ) const
{
    return m_shaders.size();
}

std::pair<dsstring,bool> LuaClass_FxParams::GetShaderFile( int p_index ) const
{
    return m_shaders[p_index];
}