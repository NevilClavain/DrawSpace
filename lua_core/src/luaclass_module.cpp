/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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
#include "luaclass_module.h"
#include "piload.h"
#include "mainservice.h"

const char LuaClass_Module::className[] = "Module";
const Luna<LuaClass_Module>::RegType LuaClass_Module::methods[] =
{
    { "load", &LuaClass_Module::LUA_load },
    { "unload", &LuaClass_Module::LUA_unload },
    { "get_name", &LuaClass_Module::LUA_getmodulename },
    { "get_descr", &LuaClass_Module::LUA_getmoduledescr },
	{ 0, 0 }
};

LuaClass_Module::LuaClass_Module( lua_State* p_L ) :
m_mod_root( NULL )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{		
        LUA_ERROR( "Module::Module : argument(s) missing" );
	}

    dsstring module_path = luaL_checkstring( p_L, 1 );
    dsstring module_instance_id = luaL_checkstring( p_L, 2 );

    m_module_path = module_path;
    m_module_instance_id = module_instance_id;
}

LuaClass_Module::~LuaClass_Module( void )
{
}

int LuaClass_Module::LUA_load( lua_State* p_L )
{
    if( !DrawSpace::Utils::PILoad::LoadModule( m_module_path, m_module_instance_id, &m_mod_root ) )
    {
        LUA_ERROR( "fail to load module" );
    }
    m_mod_root->SetHub( MainService::getInstance()->GetHub() );

    return 0;
}

int LuaClass_Module::LUA_unload( lua_State* p_L )
{
    if( !DrawSpace::Utils::PILoad::UnloadModule( m_module_path, m_mod_root ) )
    {
        LUA_ERROR( "fail to unload module" );
    }
    return 0;
}

int LuaClass_Module::LUA_getmodulename( lua_State* p_L )
{
    if( !m_mod_root )
    {
        LUA_ERROR( "module not loaded!" );
    }

    dsstring name = m_mod_root->GetModuleName();    
    lua_pushstring( p_L, name.c_str() );
    return 1;
}

int LuaClass_Module::LUA_getmoduledescr( lua_State* p_L )
{
    if( !m_mod_root )
    {
        LUA_ERROR( "module not loaded!" );
    }

    dsstring descr = m_mod_root->GetModuleDescr();    
    lua_pushstring( p_L, descr.c_str() );
    return 1;
}


DrawSpace::Interface::Module::Root* LuaClass_Module::GetModuleRoot( void ) const
{
    return m_mod_root;
}
