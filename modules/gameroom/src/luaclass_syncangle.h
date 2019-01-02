/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#ifndef _LUACLASS_SYNCANGLE_H_
#define _LUACLASS_SYNCANGLE_H_

#include "timeaspect.h"
#include "luna.h"

class LuaClass_SyncAngle
{
private:

    DrawSpace::Aspect::TimeAspect::TimeAngle    m_time_angle;

public:
	LuaClass_SyncAngle( lua_State* p_L );
	~LuaClass_SyncAngle( void );

    int LUA_initfromtimeaspectof( lua_State* p_L );
    int LUA_getvalue( lua_State* p_L );

    int LUA_inc( lua_State* p_L );
    int LUA_dec( lua_State* p_L );

    static const char className[];
    static const Luna<LuaClass_SyncAngle>::RegType methods[];
};

#endif
