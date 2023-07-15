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

#pragma once

#include <functional>

#include "ds_types.h"
#include "exceptions.h"

#include "singleton.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#define LUA_ERROR( __msg__ ) LuaContext::PushError( p_L, __msg__ )

#define LUA_TRY try

#define LUA_CATCH catch( dsexception& p_e )\
            {\
                const char* what = p_e.what();\
                char msgexcp[1024];\
                sprintf( msgexcp, "exception catch : %s", what );\
                LUA_ERROR( dsstring( msgexcp ) );\
            }

#define LUA_STDCATCH catch( std::exception& p_e )\
            {\
                const char* what = p_e.what();\
                char msgexcp[1024];\
                sprintf( msgexcp, "exception catch : %s", what );\
                LUA_ERROR( dsstring( msgexcp ) );\
            }


class LuaContext : public DrawSpace::Singleton<LuaContext>
{
protected:
	LuaContext( void );

	lua_State*					m_L;
	std::string					m_error;

    dsstring                    m_rootpath;

    inline void push_luafunc_arg( int p_val )
    {
        lua_pushinteger( m_L, p_val );
    }

    inline void push_luafunc_arg( const std::string& p_val )
    {
        lua_pushstring( m_L, p_val.c_str() );
    }

public:	
	~LuaContext( void );

    void SetRootPath( const dsstring& p_path );

	void Startup( void );
	void Shutdown( void );

	bool Execute( const dsstring& p_script );
	int ExecuteFromFile( const dsstring& p_filepath );
	dsstring GetLastError( void );
    
    template<class... Args>
    void CallLuaFunc( int p_regindex, Args&&... p_args )
    {
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, p_regindex );

        int nb_args = sizeof...(Args);

        const int a[] = {0, (process_one_type<Args>(p_args), 0)...};

        if( lua_pcall( m_L, nb_args, 0, NULL ) != 0 )
        {
            // si erreur lua (syntaxe ou autre) dans une callback function lua, faire une exception, car inutile de boucler a l'infini sur la meme erreur (notamment pour les calbacks fonction lua appelees depuis le Run())
            dsstring err_text = lua_tostring( m_L, -1 );
            _DSEXCEPTION( err_text );
        }
	};

    template<class A>
    void process_one_type( A p_a ) 
    {
        push_luafunc_arg( p_a );
    }
    
    static void PushError( lua_State* p_L, const dsstring& p_text );

    static void AddCallback( lua_State* p_L, const std::function<void(const std::string&, int)>& p_register_func );
    static void RemoveCallback( lua_State* p_L, const std::function<int(const std::string&)>& p_unregister_func );

	static int Include(lua_State* p_L);

    friend class DrawSpace::Singleton<LuaContext>;
};

