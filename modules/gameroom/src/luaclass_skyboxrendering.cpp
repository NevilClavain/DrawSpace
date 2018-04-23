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
#include "luaclass_skyboxrendering.h"
#include "luaclass_module.h"

const char LuaClass_SkyboxRendering::className[] = "SkyboxRendering";
const Luna<LuaClass_SkyboxRendering>::RegType LuaClass_SkyboxRendering::methods[] =
{
    { "instanciate_renderingimpl", &LuaClass_SkyboxRendering::LUA_instanciateRenderingImpl },
    { "trash_renderingimpl", &LuaClass_SkyboxRendering::LUA_trashRenderingImpl },
	{ 0, 0 }
};

LuaClass_SkyboxRendering::LuaClass_SkyboxRendering( lua_State* p_L ):
m_skyboxRender( NULL )
{
}

LuaClass_SkyboxRendering::~LuaClass_SkyboxRendering( void )
{
}

int LuaClass_SkyboxRendering::LUA_instanciateRenderingImpl( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "SkyboxRendering::instanciate_renderingimpl : argument(s) missing" );
	}

    LuaClass_Module* lua_mod = Luna<LuaClass_Module>::check( p_L, 1 );
    if( NULL == lua_mod )
    {
        LUA_ERROR( "SkyboxRendering::instanciate_renderingimpl : argument 1 must be of type LuaClass_Module" );
    }

    m_skyboxRender = lua_mod->GetModuleRoot()->InstanciateRenderingAspectImpls( "skyboxRender" );

    return 0;
}

int LuaClass_SkyboxRendering::LUA_trashRenderingImpl( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{		
        LUA_ERROR( "SkyboxRendering::trash_renderingimpl : argument(s) missing" );
	}

    LuaClass_Module* lua_mod = Luna<LuaClass_Module>::check( p_L, 1 );
    if( NULL == lua_mod )
    {
        LUA_ERROR( "SkyboxRendering::trash_renderingimpl : argument 1 must be of type LuaClass_Module" );
    }

    lua_mod->GetModuleRoot()->TrashRenderingAspectImpls( m_skyboxRender );

    return 0;
}