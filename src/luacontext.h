/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _LUACONTEXT_H_
#define _LUACONTEXT_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"


extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#define REGISTER_FUNC( __alias__, __func__ ) lua_register( m_L, __alias__, __func__ ); m_func_list.push_back( __alias__ );

class LuaContext
{
protected:

    static LuaContext*                                                      m_instance;

    lua_State* m_L;
    dsstring m_lasterror;
    DrawSpace::Core::BaseCallback<void, const dsstring&>*                   m_errorhandler;
    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

    std::vector<std::string> m_func_list;

    static int lua_print( lua_State* p_L );

    
    LuaContext( void );

public:    
    virtual ~LuaContext( void );

    static LuaContext* GetInstance( void )
    {
        if( NULL == m_instance )
        {
            m_instance = new LuaContext;
        }
        return m_instance;
    }

    void Startup( void );
    void Stop( void );
    bool Exec( const char* p_cmd );
    bool Execfile( const char* p_path );

    void Print( const dsstring& p_text );

    void GetLastError( dsstring& p_str );
    void RegisterErrorHandler( DrawSpace::Core::BaseCallback<void, const dsstring&>* p_handler );

    void RegisterScriptCallsHandler( DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>* p_handler );
    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>* GetScriptCallsHandler( void );
    

    lua_State* GetLuaState( void );
};


#endif
