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
#include "luaclass_orbittransform.h"
#include "luaclass_entity.h"
#include "transformaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;

const char LuaClass_OrbitTransform::className[] = "OrbitTransform";
const Luna<LuaClass_OrbitTransform>::RegType LuaClass_OrbitTransform::methods[] =
{
    { "configure", &LuaClass_OrbitTransform::LUA_configure },
    { "release", &LuaClass_OrbitTransform::LUA_release },
	{ 0, 0 }
};

LuaClass_OrbitTransform::LuaClass_OrbitTransform( lua_State* p_L )
{
}

LuaClass_OrbitTransform::~LuaClass_OrbitTransform( void )
{
}


int LuaClass_OrbitTransform::LUA_configure( lua_State* p_L )
{
    int argc = lua_gettop(p_L);
    if (argc < 10)
    {
        LUA_ERROR("OrbitTransform::configure : argument(s) missing");
    }
	
    LuaClass_Entity* lua_ent{ Luna<LuaClass_Entity>::check(p_L, 1) };
    dsreal orbit_ray{ luaL_checknumber(p_L, 2) };
    dsreal excentricity{ luaL_checknumber(p_L, 3) };
    dsreal angle{ luaL_checknumber(p_L, 4) };
    dsreal orbit_duration{ luaL_checknumber(p_L, 5) };
    dsreal orbit_offset_rot{ luaL_checknumber(p_L, 6) };
    dsreal orbit_pan_angle{ luaL_checknumber(p_L, 7) };
    dsreal orbit_tilt_angle{ luaL_checknumber(p_L, 8) };
    dsreal revol_axe_inclination{ luaL_checknumber(p_L, 9) };
    int transfoimpl_order{ luaL_checkint(p_L, 10) };

	
    DrawSpace::Core::Entity& entity{ lua_ent->GetEntity() };

    // recupere l'aspect transfo s'il existe pour cette entitee
    TransformAspect* transform_aspect{ entity.GetAspect<TransformAspect>() };
    if (transform_aspect)
    {
        transform_aspect->AddImplementation(transfoimpl_order, &m_orbit_transform);
        m_entity_transform_aspect = transform_aspect;

        m_entity_transform_aspect->AddComponent<dsreal>("orbit_ray", orbit_ray);
        m_entity_transform_aspect->AddComponent<dsreal>("orbit_excentricity", excentricity);
        m_entity_transform_aspect->AddComponent<dsreal>("orbit_angle", angle);
        m_entity_transform_aspect->AddComponent<dsreal>("orbit_duration", orbit_duration);
        m_entity_transform_aspect->AddComponent<dsreal>("orbit_offset_rot", orbit_offset_rot);
        m_entity_transform_aspect->AddComponent<dsreal>("orbit_pan_angle", orbit_pan_angle);
        m_entity_transform_aspect->AddComponent<dsreal>("orbit_tilt_angle", orbit_tilt_angle);
        m_entity_transform_aspect->AddComponent<dsreal>("revol_axe_inclination", revol_axe_inclination);       
    }
    else
    {
        m_entity_transform_aspect = NULL;
        LUA_ERROR("OrbitTransform::configure : entity passed on arg has no transform aspect");
    }
    return 0;
}

int LuaClass_OrbitTransform::LUA_release( lua_State* p_L )
{
    if( !m_entity_transform_aspect )
    {
        LUA_ERROR( "OrbitTransform::update : no transform aspect" );
    }
   
    m_entity_transform_aspect->RemoveComponent<dsreal>("orbit_ray");
    m_entity_transform_aspect->RemoveComponent<dsreal>("orbit_excentricity");
    m_entity_transform_aspect->RemoveComponent<dsreal>("orbit_angle");
    m_entity_transform_aspect->RemoveComponent<dsreal>("orbit_duration");
    m_entity_transform_aspect->RemoveComponent<dsreal>("orbit_offset_rot");
    m_entity_transform_aspect->RemoveComponent<dsreal>("orbit_pan_angle");
    m_entity_transform_aspect->RemoveComponent<dsreal>("orbit_tilt_angle");
    m_entity_transform_aspect->RemoveComponent<dsreal>("revol_axe_inclination");

    m_entity_transform_aspect->RemoveAllImplementations();
    m_entity_transform_aspect = NULL;
		
    return 0;
}