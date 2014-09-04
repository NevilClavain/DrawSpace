/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "lua_meshe.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaMeshe::className[] = "Meshe";
const DrawSpace::Luna<LuaMeshe>::RegType LuaMeshe::Register[] =
{
    { "SetObject", &LuaMeshe::Lua_SetObject },
    { "GetObject", &LuaMeshe::Lua_GetObject },
    { "InstanciateObject", &LuaMeshe::Lua_InstanciateObject },
    { "LoadFromFile", &LuaMeshe::Lua_LoadFromFile },
    { "SetImporterObject", &LuaMeshe::Lua_LoadFromFile },
    { 0 }
};


LuaMeshe::LuaMeshe( lua_State* p_L ) : 
m_meshe( NULL ),
m_release_object( false )
{

}

LuaMeshe::~LuaMeshe( void ) 
{
    cleanup();
}

void LuaMeshe::cleanup( void )
{
    if( m_meshe && m_release_object )
    {
        _DRAWSPACE_DELETE_( m_meshe );
    }
}

int LuaMeshe::Lua_SetObject( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetObject : bad number of args" );
		lua_error( p_L );		
	}

    cleanup();
    m_meshe = (Meshe*)luaL_checkinteger( p_L, 2 );
    m_release_object = false;

    return 0;
}

int LuaMeshe::Lua_GetObject( lua_State* p_L )
{
    lua_pushunsigned( p_L, (lua_Unsigned)m_meshe );
    return 1;
}

int LuaMeshe::Lua_InstanciateObject( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "InstanciateObject : bad number of args" );
		lua_error( p_L );		
	}
    
    const char* id = luaL_checkstring( p_L, 2 );

    cleanup();
    m_meshe = _DRAWSPACE_NEW_( Meshe, Meshe );
    m_release_object = true;

    LuaBindingsDirectory::GetInstance()->Register( id, this );
    m_meshe->SetName( id );
    return 0;
}

int LuaMeshe::Lua_LoadFromFile( lua_State* p_L )
{
    bool status = false;

    if( m_meshe )
    {
	    int argc = lua_gettop( p_L );
	    if( argc != 3 )
	    {
		    lua_pushstring( p_L, "LoadFromFile : bad number of args" );
		    lua_error( p_L );		
	    }

        const char* path = luaL_checkstring( p_L, 2 );
        long index = luaL_checkinteger( p_L, 3);
        status = m_meshe->LoadFromFile( path, index );
    }
    else
    {
		lua_pushstring( p_L, "LoadFromFile : refused, no associated meshe object" );
		lua_error( p_L );
    }

    lua_pushunsigned( p_L, (lua_Unsigned)status );
    return 1;
}

int LuaMeshe::Lua_SetImporterObject( lua_State* p_L )
{
    if( m_meshe )
    {
	    int argc = lua_gettop( p_L );
	    if( argc != 2 )
	    {
		    lua_pushstring( p_L, "SetImporterObject : bad number of args" );
		    lua_error( p_L );		
	    }

        DrawSpace::Interface::MesheImport* import = (DrawSpace::Interface::MesheImport*)luaL_checkinteger( p_L, 2 );

        m_meshe->SetImporter( import );
    }
    else
    {
		lua_pushstring( p_L, "SetImporterObject : refused, no associated meshe object" );
		lua_error( p_L );
    }
    return 0;
}
