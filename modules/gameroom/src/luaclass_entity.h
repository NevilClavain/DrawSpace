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

#ifndef _LUACLASS_ENTITY_H_
#define _LUACLASS_ENTITY_H_

#include "entity.h"
#include "luna.h"
#include "aspect.h"

class LuaClass_Entity
{
protected:
    DrawSpace::Core::Entity m_entity;

    typedef enum
    {
        BODY_ASPECT,
        CAMERA_ASPECT,
        PHYSICS_ASPECT,
        RENDERING_ASPECT,
        SERVICE_ASPECT,
        TIME_ASPECT,
        TRANSFORM_ASPECT,
        INFOS_ASPECT

    } AspectType;

    typedef enum
    {
        COMP_INT,
        COMP_LONG,
        COMP_DSREAL,
        COMP_FLOAT,
        COMP_DSSTRING,
        COMP_BOOL,
        COMP_TEXTDISPLAY,
    
    } ComponentType;

public:
	LuaClass_Entity( lua_State* p_L );
	~LuaClass_Entity( void );

    DrawSpace::Core::Entity& GetEntity( void );

    int LUA_addaspect( lua_State* p_L );
    int LUA_removeaspect( lua_State* p_L );

    int LUA_configuretimemmanager( lua_State* p_L );
    int LUA_readtimemmanager( lua_State* p_L );
    int LUA_updatetimemmanager( lua_State* p_L );
    int LUA_releasetimemmanager( lua_State* p_L );

    
    int LUA_configureworld( lua_State* p_L );
    int LUA_releaseworld( lua_State* p_L );

    int LUA_configurecamera( lua_State* p_L );
    int LUA_releasecamera( lua_State* p_L );

    int LUA_setupinfo(lua_State* p_L);
    int LUA_releaseinfo(lua_State* p_L);

    int LUA_connect_renderingaspect_rendergraph( lua_State* p_L );

    static const char className[];
    static const Luna<LuaClass_Entity>::RegType methods[];
};

#endif