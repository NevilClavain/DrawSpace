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

#ifndef _LUACLASS_BODY_H_
#define _LUACLASS_BODY_H_

#include "luna.h"
#include "bodyaspect.h"
#include "transformaspect.h"
#include "meshe.h"

class LuaClass_Body
{
private:
    DrawSpace::Core::Entity*                m_entity;
    DrawSpace::Aspect::TransformAspect*     m_entity_transform_aspect;
    DrawSpace::Aspect::BodyAspect*          m_entity_body_aspect;

    int                                     m_shape_type;
    DrawSpace::Aspect::BodyAspect::Mode     m_mode;
    DrawSpace::Core::Meshe                  m_meshe;
    bool                                    m_attitude_setted;
    bool                                    m_mass_setted;

public:

    int LUA_attachtoentity( lua_State* p_L );
    int LUA_detachfromentity( lua_State* p_L );

    int LUA_configureshape( lua_State* p_L );
    int LUA_configureattitude( lua_State* p_L );
    int LUA_configuremass( lua_State* p_L );
    int LUA_configureforce( lua_State* p_L );

    int LUA_configuremode( lua_State* p_L );

    int LUA_updateattitude( lua_State* p_L );
    int LUA_updateforce(lua_State* p_L);
    int LUA_updateforcestate(lua_State* p_L);

    int LUA_release( lua_State* p_L );

	LuaClass_Body( lua_State* p_L );
	~LuaClass_Body( void );

    static const char className[];
    static const Luna<LuaClass_Body>::RegType methods[];
};

#endif
