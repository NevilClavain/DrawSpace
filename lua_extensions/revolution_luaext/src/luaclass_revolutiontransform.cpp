/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#include "luacontext.h"
#include "luaclass_revolutiontransform.h"
#include "luaclass_entity.h"
#include "transformaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;

const char LuaClass_RevolutionTransform::className[] = "RevolutionTransform2";
const Luna<LuaClass_RevolutionTransform>::RegType LuaClass_RevolutionTransform::methods[] =
{
    { "configure", &LuaClass_RevolutionTransform::LUA_configure },
    { "release", &LuaClass_RevolutionTransform::LUA_release },
    { "read_currentangle", &LuaClass_RevolutionTransform::LUA_readcurrentangle },
	{ 0, 0 }
};

LuaClass_RevolutionTransform::LuaClass_RevolutionTransform( lua_State* p_L )
{
}

LuaClass_RevolutionTransform::~LuaClass_RevolutionTransform( void )
{
}


int LuaClass_RevolutionTransform::LUA_configure( lua_State* p_L )
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("RevolutionTransform::configure : argument(s) missing");
    }

    LuaClass_Entity* lua_ent{ Luna<LuaClass_Entity>::check(p_L, 1) };
    dsreal revol_duration = luaL_checknumber(p_L, 2);
	int transfoimpl_order{ luaL_checkint(p_L, 3) };
    		
    DrawSpace::Core::Entity& entity{ lua_ent->GetEntity() };

    // recupere l'aspect transfo s'il existe pour cette entitee
    TransformAspect* transform_aspect{ entity.GetAspect<TransformAspect>() };
    if (transform_aspect)
    {
        transform_aspect->AddImplementation(transfoimpl_order, &m_revolution_transform);
        m_entity_transform_aspect = transform_aspect;

        m_entity_transform_aspect->AddComponent<dsreal>("angle", 0.0);
        m_entity_transform_aspect->AddComponent<dsreal>("revol_duration", revol_duration);

    }
    else
    {
        m_entity_transform_aspect = NULL;
        LUA_ERROR("RevolutionTransform::configure : entity passed on arg has no transform aspect");
    }
    return 0;
}

int LuaClass_RevolutionTransform::LUA_release( lua_State* p_L )
{
    if( !m_entity_transform_aspect )
    {
        LUA_ERROR( "RevolutionTransform::update : no transform aspect" );
    }
   
    m_entity_transform_aspect->RemoveComponent<dsreal>("angle");
    m_entity_transform_aspect->RemoveComponent<dsreal>("revol_duration");


    m_entity_transform_aspect->RemoveAllImplementations();
    m_entity_transform_aspect = NULL;
		
    return 0;
}

int LuaClass_RevolutionTransform::LUA_readcurrentangle(lua_State* p_L)
{
    if (!m_entity_transform_aspect)
    {
        LUA_ERROR("RevolutionTransform::read_currentangle : no transform aspect");
    }

    dsreal current_angle{ m_entity_transform_aspect->GetComponent<dsreal>("angle")->getPurpose() };
    lua_pushnumber(p_L, current_angle);
    return 1;
}