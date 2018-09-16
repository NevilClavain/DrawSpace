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
#include "luaclass_rendercontext.h"


using namespace DrawSpace::Core;

const char LuaClass_RenderContext::className[] = "RenderContext";
const Luna<LuaClass_RenderContext>::RegType LuaClass_RenderContext::methods[] =
{
    { "add_fxparams", &LuaClass_RenderContext::LUA_addfxparams },
    { "add_texturesset", &LuaClass_RenderContext::LUA_addtexturesset },
    { "add_vertextexturesset", &LuaClass_RenderContext::LUA_addvertextexturesset },
    { "set_renderingorder", &LuaClass_RenderContext::LUA_setrenderingorder },
    { "add_shaderparam", &LuaClass_RenderContext::LUA_addshaderparam },
	{ 0, 0 }
};

LuaClass_RenderContext::LuaClass_RenderContext( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "RenderContext::RenderContext : argument(s) missing" );
	}

    m_data.passname = luaL_checkstring( p_L, 1 );
}

LuaClass_RenderContext::~LuaClass_RenderContext( void )
{
}

int LuaClass_RenderContext::LUA_addfxparams( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "RenderContext::add_fxparams : argument(s) missing" );
	}
    LuaClass_FxParams* lua_fx = Luna<LuaClass_FxParams>::check( p_L, 1 );

    m_data.fxparams.push_back( lua_fx->GetData() );
    return 0;
}

int LuaClass_RenderContext::LUA_addtexturesset( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "RenderContext::add_texturesset : argument(s) missing" );
	}
    LuaClass_TexturesSet* lua_textures = Luna<LuaClass_TexturesSet>::check( p_L, 1 );

    m_data.textures_sets.push_back( lua_textures->GetData() );
    return 0;
}

int LuaClass_RenderContext::LUA_addvertextexturesset( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "RenderContext::add_vertextexturesset : argument(s) missing" );
	}
    LuaClass_TexturesSet* lua_textures = Luna<LuaClass_TexturesSet>::check( p_L, 1 );

    m_data.vertex_textures_sets.push_back( lua_textures->GetData() );
    return 0;
}

int LuaClass_RenderContext::LUA_setrenderingorder( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "RenderContext::set_renderingorder : argument(s) missing" );
	}
    
    int ro = luaL_checkint( p_L, 1 );
    m_data.rendering_order = ro;

    return 0;
}

int LuaClass_RenderContext::LUA_addshaderparam( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{		
        LUA_ERROR( "RenderContext::add_shaderparam : argument(s) missing" );
	}

    dsstring shader_param_id = luaL_checkstring( p_L, 1 );
    int shader_index = luaL_checkint( p_L, 2 );
    int register_index = luaL_checkint( p_L, 3 );

    RenderingNode::ShadersParams sp;

    sp.shader_index = shader_index;
    sp.param_register = register_index;

    NamedShaderParam param = NamedShaderParam( shader_param_id, sp );

    m_data.shaders_params.push_back( param );

    return 0;
}