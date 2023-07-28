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

#include "luacontext.h"
#include "luaclass_fpsmvttransform.h"
#include "luaclass_entity.h"
#include "transformaspect.h"
#include "quaternion.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;

const char LuaClass_FPSMovementTransform::className[] = "FPSMovementTransform";
const Luna<LuaClass_FPSMovementTransform>::RegType LuaClass_FPSMovementTransform::methods[] =
{
    { "configure", &LuaClass_FPSMovementTransform::LUA_configure },
    { "update", &LuaClass_FPSMovementTransform::LUA_update },
    { "read", &LuaClass_FPSMovementTransform::LUA_read },
    { "release", &LuaClass_FPSMovementTransform::LUA_release },
	{ 0, 0 }
};

LuaClass_FPSMovementTransform::LuaClass_FPSMovementTransform( lua_State* p_L )
{
}

LuaClass_FPSMovementTransform::~LuaClass_FPSMovementTransform( void )
{
}


int LuaClass_FPSMovementTransform::LUA_configure( lua_State* p_L )
{
    int argc = lua_gettop(p_L);
    if (argc < 8)
    {
        LUA_ERROR("FPSMovementTransform::configure : argument(s) missing");
    }

    LuaClass_Entity* lua_ent{ Luna<LuaClass_Entity>::check(p_L, 1) };

    dsreal yaw              { luaL_checknumber(p_L, 2) };
    dsreal pitch            { luaL_checknumber(p_L, 3) };

    dsreal x                { luaL_checknumber(p_L, 4) };
    dsreal y                { luaL_checknumber(p_L, 5) };
    dsreal z                { luaL_checknumber(p_L, 6) };

    int ymvt                { luaL_checkint(p_L, 7) };
    int transfoimpl_order   { luaL_checkint(p_L, 8) };
		
    DrawSpace::Core::Entity& entity{ lua_ent->GetEntity() };

    // recupere l'aspect transfo s'il existe pour cette entitee
    TransformAspect* transform_aspect{ entity.GetAspect<TransformAspect>() };
    if (transform_aspect)
    {
        transform_aspect->AddImplementation(transfoimpl_order, this);
        m_entity_transform_aspect = transform_aspect;

        LUA_TRY
        {
            transform_aspect->AddComponent<dsreal>("yaw", yaw);
            transform_aspect->AddComponent<dsreal>("pitch", pitch);

            transform_aspect->AddComponent<Vector>("speed");
            transform_aspect->AddComponent<Matrix>("pos");

            transform_aspect->GetComponent<Matrix>("pos")->getPurpose().Translation(Vector(x, y, z, 1.0));

            transform_aspect->AddComponent<bool>("ymvt", static_cast<bool>( ymvt ) );

        } LUA_CATCH;
    }
    else
    {
        m_entity_transform_aspect = NULL;
        LUA_ERROR("FPSMovementTransform::configure : entity passed on arg has no transform aspect");
    }
    return 0;
}

int LuaClass_FPSMovementTransform::LUA_release( lua_State* p_L )
{
    if( !m_entity_transform_aspect )
    {
        LUA_ERROR( "FPSMovementTransform::update : no transform aspect" );
    }

    m_entity_transform_aspect->RemoveComponent<dsreal>("yaw");
    m_entity_transform_aspect->RemoveComponent<dsreal>("pitch");

    m_entity_transform_aspect->RemoveComponent<Vector>("speed");
    m_entity_transform_aspect->RemoveComponent<Matrix>("pos");

    m_entity_transform_aspect->RemoveComponent<bool>("ymvt");
    m_entity_transform_aspect->RemoveAllImplementations();
    m_entity_transform_aspect = NULL;
		
    return 0;
}

int LuaClass_FPSMovementTransform::LUA_update(lua_State* p_L)
{
    if (!m_entity_transform_aspect)
    {
        LUA_ERROR("FPSTransform::update : no transform aspect");
    }

    int argc = lua_gettop(p_L);
    if (argc < 9)
    {
        LUA_ERROR("FPSTransform::update : argument(s) missing");
    }

    LUA_TRY
    {
        dsreal yaw = luaL_checknumber(p_L, 1);
        dsreal pitch = luaL_checknumber(p_L, 2);

        dsreal x = luaL_checknumber(p_L, 3);
        dsreal y = luaL_checknumber(p_L, 4);
        dsreal z = luaL_checknumber(p_L, 5);

        bool ymvt = luaL_checkint(p_L, 6);

        dsreal xspeed = luaL_checknumber(p_L, 7);
        dsreal yspeed = luaL_checknumber(p_L, 8);
        dsreal zspeed = luaL_checknumber(p_L, 9);


        m_entity_transform_aspect->GetComponent<dsreal>("yaw")->getPurpose() = yaw;
        m_entity_transform_aspect->GetComponent<dsreal>("pitch")->getPurpose() = pitch;

        m_entity_transform_aspect->GetComponent<Matrix>("pos")->getPurpose().Translation(Vector(x, y, z, 1.0));

        m_entity_transform_aspect->GetComponent<bool>("ymvt")->getPurpose() = ymvt;

        Vector speed(xspeed, yspeed, zspeed, 1.0);
        m_entity_transform_aspect->GetComponent<Vector>("speed")->getPurpose() = speed;

    } LUA_CATCH;

    return 0;
}

int LuaClass_FPSMovementTransform::LUA_read(lua_State* p_L)
{
    if (!m_entity_transform_aspect)
    {
        LUA_ERROR("FPSTransform::read : no transform aspect");
    }

    LUA_TRY
    {

        dsreal yaw = m_entity_transform_aspect->GetComponent<dsreal>("yaw")->getPurpose();
        dsreal pitch = m_entity_transform_aspect->GetComponent<dsreal>("pitch")->getPurpose();

        Matrix mat = m_entity_transform_aspect->GetComponent<Matrix>("pos")->getPurpose();

        dsreal x = mat(3, 0);
        dsreal y = mat(3, 1);
        dsreal z = mat(3, 2);

        bool ymvt = m_entity_transform_aspect->GetComponent<bool>("ymvt")->getPurpose();

        Vector speed = m_entity_transform_aspect->GetComponent<Vector>("speed")->getPurpose();

        lua_pushnumber(p_L, yaw);
        lua_pushnumber(p_L, pitch);
        lua_pushnumber(p_L, x);
        lua_pushnumber(p_L, y);
        lua_pushnumber(p_L, z);
        lua_pushinteger(p_L, ymvt);

        lua_pushnumber(p_L, speed[0]);
        lua_pushnumber(p_L, speed[1]);
        lua_pushnumber(p_L, speed[2]);

    } LUA_CATCH;

    return 9;
}

void LuaClass_FPSMovementTransform::GetLocaleTransform(DrawSpace::Aspect::TransformAspect* p_transformaspect, DrawSpace::Utils::Matrix& p_out_base_transform)
{
    if (NULL == m_time_aspect)
    {
        _DSEXCEPTION("Need a time reference!!!")
    }

    // recup des composants donnees d'entrees
    ComponentList<dsreal> angles;
    p_transformaspect->GetComponentsByType<dsreal>(angles);

    dsreal angle_yaw = angles[0]->getPurpose();
    dsreal angle_pitch = angles[1]->getPurpose();

    ComponentList<Vector> vectors;
    p_transformaspect->GetComponentsByType<Vector>(vectors);

    Vector local_speed = vectors[0]->getPurpose();

    local_speed[2] = -local_speed[2];

    ComponentList<Matrix> mats;
    p_transformaspect->GetComponentsByType<Matrix>(mats);

    Matrix pos = mats[0]->getPurpose();

    ComponentList<bool> flags;
    p_transformaspect->GetComponentsByType<bool>(flags);

    bool y_mvt = flags[0]->getPurpose();

    // les quaternions
    Maths::Quaternion		    qyaw;
    Maths::Quaternion		    qpitch;
    Maths::Quaternion		    current_res;

    // les sorties
    Utils::Matrix			    orientation;

    Vector gs;

    Vector yaxis(0.0, 1.0, 0.0, 1.0);
    Vector xaxis(1.0, 0.0, 0.0, 1.0);

    qyaw.rotationAxis(yaxis, angle_yaw);
    qpitch.rotationAxis(xaxis, angle_pitch);

    current_res = qpitch * qyaw;
    current_res.rotationMatFrom(orientation);

    orientation.Transform(&local_speed, &gs);

    TimeAspect::TimeScalar pos_30 = m_time_aspect->TimeScalarFactory(pos(3, 0));
    TimeAspect::TimeScalar pos_31 = m_time_aspect->TimeScalarFactory(pos(3, 1));
    TimeAspect::TimeScalar pos_32 = m_time_aspect->TimeScalarFactory(pos(3, 2));

    pos_30 += gs[0];

    if (y_mvt)
    {
        // prendre aussi en compte la composante en Y (la camera peut aussi evoluer "en hauteur")
        pos_31 += gs[1];
    }

    pos_32 += gs[2];

    pos(3, 0) = pos_30.GetValue();
    pos(3, 1) = pos_31.GetValue();
    pos(3, 2) = pos_32.GetValue();

    p_out_base_transform = orientation * pos;

    mats[0]->getPurpose() = pos;
}
