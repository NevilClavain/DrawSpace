/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2021
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
#include "luaclass_rigidbodytransform.h"
#include "luaclass_entity.h"
#include "luaclass_matrix.h"
#include "luaclass_vector.h"
#include "timeaspect.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;


const char LuaClass_RigidBodyTransform::className[] = "RigdBodyTransform";
const Luna<LuaClass_RigidBodyTransform>::RegType LuaClass_RigidBodyTransform::methods[] =
{
    { "attach_toentity", &LuaClass_RigidBodyTransform::LUA_attachtoentity },

    { "configure_shape", &LuaClass_RigidBodyTransform::LUA_configureshape },
    { "configure_attitude", &LuaClass_RigidBodyTransform::LUA_configureattitude },
    { "configure_mass", &LuaClass_RigidBodyTransform::LUA_configuremass },

    { "configure_force", &LuaClass_RigidBodyTransform::LUA_configureforce },
    { "configure_torque", &LuaClass_RigidBodyTransform::LUA_configuretorque },

    { "update_attitude", &LuaClass_RigidBodyTransform::LUA_updateattitude },
    { "update_force", &LuaClass_RigidBodyTransform::LUA_updateforce },
    { "update_forcestate", &LuaClass_RigidBodyTransform::LUA_updateforcestate },

    { "update_torce", &LuaClass_RigidBodyTransform::LUA_updatetorque },
    { "update_torquestate", &LuaClass_RigidBodyTransform::LUA_updatetorquestate },

    { "zero_speed", &LuaClass_RigidBodyTransform::LUA_zerospeed },
    { "zero_angularspeed", &LuaClass_RigidBodyTransform::LUA_zeroangularspeed },
    { "release", &LuaClass_RigidBodyTransform::LUA_release },

    { 0, 0 }
};


LuaClass_RigidBodyTransform::LuaClass_RigidBodyTransform(lua_State* p_L)
{
}

LuaClass_RigidBodyTransform::~LuaClass_RigidBodyTransform(void)
{
}

int LuaClass_RigidBodyTransform::LUA_attachtoentity(lua_State* p_L)
{
    int argc{ lua_gettop(p_L) };
    if (argc < 2)
    {
        LUA_ERROR("RigdBodyTransform::attach_toentity : argument(s) missing");
    }
   
    LuaClass_Entity* lua_ent{ Luna<LuaClass_Entity>::check(p_L, 1) };
    int transfoimpl_order = luaL_checkint(p_L, 2);

    DrawSpace::Core::Entity& entity = lua_ent->GetEntity();

    TransformAspect* transform_aspect = entity.GetAspect<TransformAspect>();
    if (NULL == transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::attach_toentity : entity has no transform aspect!");
    }

    m_entity_transform_aspect = transform_aspect;
    m_entity = &entity;

    m_entity_transform_aspect->AddImplementation(transfoimpl_order, &m_rigidbody_transformer);

    // add bool component for contact state
    m_entity_transform_aspect->AddComponent<bool>("contact_state", false);

    // add bool component for linear speed stop
    m_entity_transform_aspect->AddComponent<bool>("stop_linear_speed", false);

    // add bool component for angular speed stop
    m_entity_transform_aspect->AddComponent<bool>("stop_angular_speed", false);

    return 0;
}

int LuaClass_RigidBodyTransform::LUA_configureshape(lua_State* p_L)
{
    if (NULL == m_entity)
    {
        LUA_ERROR("RigdBodyTransform::configure_shape : no attached entity");
    }

    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("RigdBodyTransform::configure_shape : argument(s) missing");
    }

    m_shape_type = luaL_checkint(p_L, 1);

    if (NULL == m_entity_transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::configure_shape : no transform aspect");
    }

    switch (m_shape_type)
    {
        case 0: // SHAPE_BOX
        {
            if (argc < 4)
            {
                LUA_ERROR("RigdBodyTransform::configure_shape : argument(s) missing");
            }
            dsreal xdim = luaL_checknumber(p_L, 2);
            dsreal ydim = luaL_checknumber(p_L, 3);
            dsreal zdim = luaL_checknumber(p_L, 4);

            m_entity_transform_aspect->AddComponent<RigidBodyTransformAspectImpl::BoxCollisionShape>("shape", Vector(xdim, ydim, zdim, 1.0));
        }
        break;

        case 1: // SHAPE_SPHERE
        {
            if (argc < 2)
            {
                LUA_ERROR("RigdBodyTransform::configure_shape : argument(s) missing");
            }
            dsreal ray = luaL_checknumber(p_L, 2);

            m_entity_transform_aspect->AddComponent<RigidBodyTransformAspectImpl::SphereCollisionShape>("shape", ray);
        }
        break;
    }

    return 0;
}

int LuaClass_RigidBodyTransform::LUA_configureattitude(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("RigdBodyTransform::configure_attitude : argument(s) missing");
    }
    LuaClass_Matrix* lua_mat = Luna<LuaClass_Matrix>::check(p_L, 1);

    if (NULL == m_entity_transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::configure_attitude : no transform aspect");
    }

    m_entity_transform_aspect->AddComponent<Matrix>("attitude", lua_mat->GetMatrix());
    m_attitude_setted = true;

    return 0;
}

int LuaClass_RigidBodyTransform::LUA_configuremass(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("RigdBodyTransform::configure_mass : argument(s) missing");
    }

    dsreal mass = luaL_checknumber(p_L, 1);

    if (NULL == m_entity_transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::configure_mass : no transform aspect");
    }

    m_entity_transform_aspect->AddComponent<dsreal>("mass", mass);
    m_mass_setted = true;

    return 0;
}

int LuaClass_RigidBodyTransform::LUA_configureforce(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 4)
    {
        LUA_ERROR("RigdBodyTransform::configure_force : argument(s) missing");
    }

    dsstring forceid = luaL_checkstring(p_L, 1);

    LuaClass_Vector* lua_vec = Luna<LuaClass_Vector>::check(p_L, 2);

    int forcemode = luaL_checkint(p_L, 3);
    bool enabled = luaL_checkint(p_L, 4);


    if (NULL == m_entity_transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::configure_force : no transform aspect");
    }

    m_entity_transform_aspect->AddComponent<RigidBodyTransformAspectImpl::Force>(forceid, lua_vec->getVector(), (RigidBodyTransformAspectImpl::Force::Mode)forcemode, enabled);
    m_forces_id.push_back(forceid);

    return 0;
}

int LuaClass_RigidBodyTransform::LUA_configuretorque(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 4)
    {
        LUA_ERROR("RigdBodyTransform::configure_torque : argument(s) missing");
    }

    dsstring torqueid = luaL_checkstring(p_L, 1);

    LuaClass_Vector* lua_vec = Luna<LuaClass_Vector>::check(p_L, 2);

    int torquemode = luaL_checkint(p_L, 3);
    bool enabled = luaL_checkint(p_L, 4);

    if (NULL == m_entity_transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::configure_torque : no transform aspect");
    }

    m_entity_transform_aspect->AddComponent<RigidBodyTransformAspectImpl::Torque>(torqueid, lua_vec->getVector(), (RigidBodyTransformAspectImpl::Torque::Mode)torquemode, enabled);
    m_torques_id.push_back(torqueid);

    return 0;
}

int LuaClass_RigidBodyTransform::LUA_updateattitude(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("RigdBodyTransform::update_attitude : argument(s) missing");
    }
    LuaClass_Matrix* lua_mat = Luna<LuaClass_Matrix>::check(p_L, 1);

    if (NULL == m_entity_transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::update_attitude : no transform aspect");
    }

    m_entity_transform_aspect->GetComponent<Matrix>("attitude")->getPurpose() = lua_mat->GetMatrix();

    return 0;
}

int LuaClass_RigidBodyTransform::LUA_updateforce(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("RigdBodyTransform::update_force : argument(s) missing");
    }

    dsstring forceid = luaL_checkstring(p_L, 1);
    LuaClass_Vector* lua_vec = Luna<LuaClass_Vector>::check(p_L, 2);

    if (NULL == m_entity_transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::update_force : no transform aspect");
    }

    m_entity_transform_aspect->GetComponent<RigidBodyTransformAspectImpl::Force>(forceid)->getPurpose().UpdateForce(lua_vec->getVector());

    return 0;
}

int LuaClass_RigidBodyTransform::LUA_updateforcestate(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("RigdBodyTransform::update_forcestate : argument(s) missing");
    }

    dsstring forceid = luaL_checkstring(p_L, 1);
    bool enabled = luaL_checkint(p_L, 2);

    if (NULL == m_entity_transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::update_forcestate : no transform aspect");
    }

    if (enabled)
    {
        m_entity_transform_aspect->GetComponent<RigidBodyTransformAspectImpl::Force>(forceid)->getPurpose().Enable();
    }
    else
    {
        m_entity_transform_aspect->GetComponent<RigidBodyTransformAspectImpl::Force>(forceid)->getPurpose().Disable();
    }

    return 0;
}

int LuaClass_RigidBodyTransform::LUA_updatetorque(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("RigdBodyTransform::update_torque : argument(s) missing");
    }

    dsstring torqueid = luaL_checkstring(p_L, 1);
    LuaClass_Vector* lua_vec = Luna<LuaClass_Vector>::check(p_L, 2);

    if (NULL == m_entity_transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::update_torque : no transform aspect");
    }

    m_entity_transform_aspect->GetComponent<RigidBodyTransformAspectImpl::Torque>(torqueid)->getPurpose().UpdateForce(lua_vec->getVector());

    return 0;
}

int LuaClass_RigidBodyTransform::LUA_updatetorquestate(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("RigdBodyTransform::update_torquestate : argument(s) missing");
    }

    dsstring torqueid = luaL_checkstring(p_L, 1);
    bool enabled = luaL_checkint(p_L, 2);

    if (NULL == m_entity_transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::update_torquestate : no transform aspect");
    }

    if (enabled)
    {
        m_entity_transform_aspect->GetComponent<RigidBodyTransformAspectImpl::Torque>(torqueid)->getPurpose().Enable();
    }
    else
    {
        m_entity_transform_aspect->GetComponent<RigidBodyTransformAspectImpl::Torque>(torqueid)->getPurpose().Disable();
    }

    return 0;
}

int LuaClass_RigidBodyTransform::LUA_zerospeed(lua_State* p_L)
{
    if (NULL == m_entity_transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::zero_speed : no transform aspect");
    }

    m_entity_transform_aspect->GetComponent<bool>("stop_linear_speed")->getPurpose() = true;
    return 0;
}

int LuaClass_RigidBodyTransform::LUA_zeroangularspeed(lua_State* p_L)
{
    if (NULL == m_entity_transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::zero_angularspeed : no transform aspect");
    }
    m_entity_transform_aspect->GetComponent<bool>("stop_angular_speed")->getPurpose() = true;
    return 0;
}

int LuaClass_RigidBodyTransform::LuaClass_RigidBodyTransform::LUA_release(lua_State* p_L)
{
    if (NULL == m_entity)
    {
        LUA_ERROR("RigdBodyTransform::release : no related entity");
    }

    if (NULL == m_entity_transform_aspect)
    {
        LUA_ERROR("RigdBodyTransform::release : no transform aspect");
    }

    m_entity_transform_aspect->RemoveComponent<bool>("contact_state");
    m_entity_transform_aspect->RemoveComponent<bool>("stop_linear_speed");
    m_entity_transform_aspect->RemoveComponent<bool>("stop_angular_speed");

    if (m_shape_type != -1)
    {
        switch (m_shape_type)
        {
            case 0: // SHAPE_BOX
            {
                m_entity_transform_aspect->RemoveComponent<RigidBodyTransformAspectImpl::BoxCollisionShape>("shape");
            }
            break;

            case 1: // SHAPE_SPHERE
            {
                m_entity_transform_aspect->RemoveComponent<RigidBodyTransformAspectImpl::SphereCollisionShape>("shape");
            }
            break;
        }
        m_shape_type = -1;
    }

    // release forces & torques
    for (auto& e : m_forces_id)
    {
        m_entity_transform_aspect->RemoveComponent<Matrix>(e);
    }
    for (auto& e : m_torques_id)
    {
        m_entity_transform_aspect->RemoveComponent<Matrix>(e);
    }

    if (m_attitude_setted)
    {
        m_entity_transform_aspect->RemoveComponent<Matrix>("attitude");
        m_attitude_setted = false;
    }

    if (m_mass_setted)
    {
        m_entity_transform_aspect->RemoveComponent<dsreal>("mass");
        m_mass_setted = false;
    }

    //m_rigidbody_transformer.Release();

    m_entity_transform_aspect->RemoveAllImplementations();
    m_entity_transform_aspect = nullptr;
    m_entity = NULL;

    return 0;
}
