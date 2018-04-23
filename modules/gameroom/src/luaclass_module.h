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

#ifndef _LUACLASS_MODULE_H_
#define _LUACLASS_MODULE_H_

#include "luna.h"
#include "module_root.h"

class LuaClass_Module
{
private:
    dsstring m_module_path;
    dsstring m_module_instance_id;

    DrawSpace::Interface::Module::Root* m_mod_root;

public:
	LuaClass_Module( lua_State* p_L );
	~LuaClass_Module( void );

    int LUA_load( lua_State* p_L );
    int LUA_getmodulename( lua_State* p_L );
    int LUA_getmoduledescr( lua_State* p_L );
    int LUA_getserviceslist( lua_State* p_L );

    DrawSpace::Interface::Module::Root* GetModuleRoot( void ) const;

    static const char className[];
    static const Luna<LuaClass_Module>::RegType methods[];
};

#endif