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

#ifndef _LUA_CONTEXT_H_
#define _LUA_CONTEXT_H_

#include "drawspace_commons.h"
#include "callback.h"

extern "C" {
#include "luastack\lua.h"
#include "luastack\lualib.h"
#include "luastack\lauxlib.h"
};

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <map>


#define REGISTER_FUNC( __alias__, __func__ ) lua_register( m_L, __alias__, __func__ ); m_func_list.push_back( __alias__ );


namespace DrawSpace
{
class LuaContext
{
protected:
    lua_State* m_L;

    dsstring m_lasterror;

    DrawSpace::Core::BaseCallback<void, const dsstring&>*     m_errorhandler;

public:
    LuaContext( void );
    virtual ~LuaContext( void );

    void Startup( void );
    void Stop( void );
    void Exec( const char* p_cmd );
    void Execfile( const char* p_path );

    void GetLastError( dsstring& p_str );
    void RegisterErrorHandler( DrawSpace::Core::BaseCallback<void, const dsstring&>* p_handler );

    lua_State* GetLuaState( void );
};
}

#endif