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

#include "lua_context.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

LuaContext::LuaContext( void ) : m_errorhandler( NULL )
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
}

void LuaContext::Stop( void )
{
	lua_close( m_L );
}

void LuaContext::Exec( const char* p_cmd )
{
	int status = luaL_dostring( m_L, p_cmd );

	if( status )
	{
		char* err_msg = (char *)lua_tostring( m_L, -1 );
        //printf( "error : %s\n", err_msg );
        m_lasterror = err_msg;
		// popper le message d'erreur
		lua_pop( m_L, 1 );

        if( m_errorhandler )
        {
            (*m_errorhandler)( m_lasterror );
        }
	}
}

void LuaContext::Execfile( const char* p_path )
{    
	int status = luaL_dofile( m_L, p_path );

	if( status )
	{
		char* err_msg = (char *)lua_tostring( m_L, -1 );
		//printf( "error : %s\n", err_msg );
        m_lasterror = err_msg;
        // popper le message d'erreur
		lua_pop( m_L, 1 );

        if( m_errorhandler )
        {
            (*m_errorhandler)( m_lasterror );
        }
	}    
}

void LuaContext::GetLastError( dsstring& p_str )
{
    p_str = m_lasterror;
}

void LuaContext::RegisterErrorHandler( BaseCallback<void, const dsstring&>* p_handler )
{
    m_errorhandler = p_handler;
}

lua_State* LuaContext::GetLuaState( void )
{
    return m_L;
}

///////////////////////////////////////////////

