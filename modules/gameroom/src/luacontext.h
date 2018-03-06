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

#ifndef _LUACONTEXT_H_
#define _LUACONTEXT_H_

#include "drawspace_commons.h"

#include "crtp_singleton.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};


class LuaContext : public BaseSingleton<LuaContext>
{
protected:
	LuaContext( void );

	lua_State*					m_L;
	std::string					m_error;
public:	
	~LuaContext( void );

	void Startup( void );
	void Shutdown( void );

	bool Execute( const std::string& p_script );
	bool ExecuteFromFile( const std::string& p_fichier );
	dsstring GetLastError( void );

    friend class BaseSingleton<LuaContext>;
};

#endif