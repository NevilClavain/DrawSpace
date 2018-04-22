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

#include "file.h"
#include "luacontext.h"
#include "luaclass_globals.h"
#include "luaclass_renderpassnodegraph.h"
#include "luaclass_renderer.h"
#include "luaclass_gui.h"
#include "luaclass_renderstatesset.h"
#include "luaclass_renderassembly.h"
#include "luaclass_entity.h"
#include "luaclass_entitynodegraph.h"
#include "luaclass_fpstransformaspectimpl.h"
#include "luaclass_textrenderingaspectimpl.h"
#include "luaclass_mesherenderingaspectimpl.h"
#include "luaclass_syncangle.h"
#include "luaclass_syncscalar.h"
#include "luaclass_texturesset.h"
#include "luaclass_fxparams.h"
#include "luaclass_rendercontext.h"
#include "luaclass_renderconfig.h"
#include "luaclass_skyboxrendering.h"
#include "luaclass_module.h"

LuaContext::LuaContext( void ) :
m_L( NULL )
{
}

LuaContext::~LuaContext( void )
{
}

void LuaContext::Startup( void )
{
	m_L = luaL_newstate();
	
	luaopen_io( m_L );
    luaopen_base( m_L );
    luaopen_table( m_L );
    luaopen_string( m_L );

    Luna<LuaClass_Globals>::Register( m_L );
    Luna<LuaClass_RenderPassNodeGraph>::Register( m_L );
    Luna<LuaClass_Renderer>::Register( m_L );
    Luna<LuaClass_Gui>::Register( m_L );
    Luna<LuaClass_RenderStatesSet>::Register( m_L );
    Luna<LuaClass_TexturesSet>::Register( m_L );
    Luna<LuaClass_FxParams>::Register( m_L );
    Luna<LuaClass_RenderAssembly>::Register( m_L );
    Luna<LuaClass_Entity>::Register( m_L );
    Luna<LuaClass_EntityNodeGraph>::Register( m_L );
    Luna<LuaClass_FPSTransformAspectImpl>::Register( m_L );
    Luna<LuaClass_TextRenderingAspectImpl>::Register( m_L );
    Luna<LuaClass_MesheRenderingAspectImpl>::Register( m_L );
    Luna<LuaClass_SkyboxRendering>::Register( m_L );
    Luna<LuaClass_SyncAngle>::Register( m_L );
    Luna<LuaClass_SyncScalar>::Register( m_L );
    Luna<LuaClass_RenderContext>::Register( m_L );
    Luna<LuaClass_RenderConfig>::Register( m_L );
    Luna<LuaClass_Module>::Register( m_L );
}

void LuaContext::Shutdown( void )
{
    if( m_L )
    {
        lua_close( m_L );
        m_L = NULL;
    }
}

bool LuaContext::Execute( const std::string& p_script )
{
	int status = luaL_dostring( m_L, p_script.c_str() );
	if( status )
	{
        m_error = lua_tostring( m_L, -1 );
		// popper le message d'erreur
		lua_pop( m_L, 1 );
        return false;
	}
    return true;
}

int LuaContext::ExecuteFromFile( const std::string& p_filepath )
{
    int global_status = 0;
    long fsize;
    void* content = DrawSpace::Utils::File::LoadAndAllocBinaryFile( p_filepath, &fsize );
    if( !content )
    {
        global_status = -1;
        return global_status;
    }

    char* text = new char[fsize + 1];
    memcpy( text, content, fsize );
    text[fsize] = 0;

    int status = luaL_dostring( m_L, text );

    _DRAWSPACE_DELETE_N_( content );
    delete[] text;
	if( status )
	{
        m_error = lua_tostring( m_L, -1 );
		// popper le message d'erreur
		lua_pop( m_L, 1 );

        global_status = -2;
	}
	return global_status;
}

dsstring LuaContext::GetLastError( void )
{
    return m_error;
}

void LuaContext::PushError( lua_State* p_L, const dsstring& p_text )
{
	lua_pushstring( p_L, p_text.c_str() );
	lua_error( p_L );
}

void LuaContext::AddCallback( lua_State* p_L, const std::function<void(const std::string&, int)>& p_register_func )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
        LUA_ERROR( "AddCallback : argument(s) missing" );
	}

    dsstring cbid = luaL_checkstring( p_L, 1 );
    int status = lua_isfunction( p_L, 2 );

    if( status > 0 )
    {
        int reffunc = luaL_ref( p_L, LUA_REGISTRYINDEX );     
        
        p_register_func( cbid, reffunc );
    }
    else
    {
        LUA_ERROR( "AddCallback : argument 2 must be a function" );
    }
}

void LuaContext::RemoveCallback( lua_State* p_L, const std::function<int(const std::string&)>& p_unregister_func )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "RemoveCallback : argument(s) missing" );
	}
    dsstring cbid = luaL_checkstring( p_L, 1 );

    int reffunc = p_unregister_func( cbid );
    if( -1 == reffunc )
    {
        LUA_ERROR( "RemoveCallback : unknown callback id" );
    }
    else
    {
        // liberer la ref...
        luaL_unref( p_L, LUA_REGISTRYINDEX, reffunc );
    }
}