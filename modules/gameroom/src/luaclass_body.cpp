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
#include "luaclass_body.h"
#include "luaclass_entity.h"
#include "luaclass_matrix.h"
#include "luaclass_vector.h"

#include "mainservice.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;

const char LuaClass_Body::className[] = "Body";
const Luna<LuaClass_Body>::RegType LuaClass_Body::methods[] =
{
    { "attach_toentity", &LuaClass_Body::LUA_attachtoentity },
    { "detach_fromentity", &LuaClass_Body::LUA_detachfromentity },

    { "configure_shape", &LuaClass_Body::LUA_configureshape },
    { "configure_attitude", &LuaClass_Body::LUA_configureattitude },
    { "configure_mass", &LuaClass_Body::LUA_configuremass },
    { "configure_mode", &LuaClass_Body::LUA_configuremode },

    { "configure_force", &LuaClass_Body::LUA_configureforce },
    { "configure_torque", &LuaClass_Body::LUA_configuretorque },

    { "update_attitude", &LuaClass_Body::LUA_updateattitude },
    { "update_force", &LuaClass_Body::LUA_updateforce },
    { "update_forcestate", &LuaClass_Body::LUA_updateforcestate },

    { "update_torce", &LuaClass_Body::LUA_updatetorque },
    { "update_torquestate", &LuaClass_Body::LUA_updatetorquestate },

    { "zero_speed", &LuaClass_Body::LUA_zerospeed },
    { "zero_angularespeed", &LuaClass_Body::LUA_zeroangularspeed },

    { "release", &LuaClass_Body::LUA_release },
	{ 0, 0 }
};

LuaClass_Body::LuaClass_Body( lua_State* p_L ) :
m_entity( NULL ),
m_entity_transform_aspect( NULL ),
m_entity_body_aspect( NULL ),
m_shape_type( -1 ),
m_mode( BodyAspect::NOT_READY ),
m_attitude_setted( false ),
m_mass_setted( false )
{
    //m_meshe.SetImporter( MainService::GetInstance()->GetMesheImport() );
}

LuaClass_Body::~LuaClass_Body( void )
{
}

int LuaClass_Body::LUA_attachtoentity( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{		
        LUA_ERROR( "Body::attach_toentity : argument(s) missing" );
	}

    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 1 );
    int transfoimpl_order = luaL_checkint(p_L, 2);

    DrawSpace::Core::Entity& entity = lua_ent->GetEntity();

    TransformAspect* transform_aspect = entity.GetAspect<TransformAspect>();
    if( NULL == transform_aspect )
    {
        LUA_ERROR( "Body::attach_toentity : entity has no transform aspect!" );
    }

    BodyAspect* body_aspect = entity.GetAspect<BodyAspect>();
    if( NULL == body_aspect )
    {
        LUA_ERROR( "Body::attach_toentity : entity has no body aspect!" );
    }

    m_entity_transform_aspect = transform_aspect;
    m_entity_body_aspect = body_aspect;
    m_entity = &entity;

    // bind transfo and body aspects
    //m_entity_transform_aspect->SetImplementation( m_entity_body_aspect->GetTransformAspectImpl() );
    m_entity_transform_aspect->AddImplementation(transfoimpl_order, m_entity_body_aspect->GetTransformAspectImpl());

    // add bool component for contact state
    m_entity_body_aspect->AddComponent<bool>( "contact_state", false );

    // add bool component for linear speed stop
    m_entity_body_aspect->AddComponent<bool>("stop_linear_speed", false);

    // add bool component for angular speed stop
    m_entity_body_aspect->AddComponent<bool>("stop_angular_speed", false);


    return 0;
}

int LuaClass_Body::LUA_detachfromentity( lua_State* p_L )
{
    if( NULL == m_entity )
    {
        LUA_ERROR( "Body::detach_fromentity : argument(s) missing" );
    }

    m_entity_body_aspect->RemoveComponent<bool>( "contact_state" );
    m_entity_body_aspect->RemoveComponent<bool>("stop_linear_speed");
    m_entity_body_aspect->RemoveComponent<bool>("stop_angular_speed");

    m_entity_transform_aspect->RemoveAllImplementations();

    m_entity_body_aspect = NULL;
    m_entity_transform_aspect = NULL;
    m_entity = NULL;

    return 0;
}

int LuaClass_Body::LUA_configureshape( lua_State* p_L )
{
    if (NULL == m_entity)
    {
        LUA_ERROR("Body::configure_shape : no attached entity");
    }
    ResourcesAspect* resources_aspect = m_entity->GetAspect<ResourcesAspect>();
    if (!resources_aspect)
    {
        LUA_ERROR("Body::configure_shape : attached entity has no resources aspect !");
    }

	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Body::configure_shape : argument(s) missing" );
	}

    m_shape_type = luaL_checkint( p_L, 1 );

    if( NULL == m_entity_body_aspect )
    {
        LUA_ERROR( "Body::configure_shape : no body aspect" );
    }

    switch( m_shape_type )
    {
        case 0: // SHAPE_BOX
        {
	        if( argc < 4 )
	        {
                LUA_ERROR( "Body::configure_shape : argument(s) missing" );
	        }
            dsreal xdim = luaL_checknumber( p_L, 2 );
            dsreal ydim = luaL_checknumber( p_L, 3 );
            dsreal zdim = luaL_checknumber( p_L, 4 );

            m_entity_body_aspect->AddComponent<BodyAspect::BoxCollisionShape>( "shape", Vector( xdim, ydim, zdim, 1.0 ) );
        }
        break;

        case 1: // SHAPE_SPHERE
        {
	        if( argc < 2 )
	        {
                LUA_ERROR( "Body::configure_shape : argument(s) missing" );
	        }
            dsreal ray = luaL_checknumber( p_L, 2 );

            m_entity_body_aspect->AddComponent<BodyAspect::SphereCollisionShape>( "shape", ray );
        }
        break;

        case 2: // SHAPE_MESHE
        {
	        if( argc < 3 )
	        {
                LUA_ERROR( "Body::configure_shape : argument(s) missing" );
	        }
            dsstring meshe_path = luaL_checkstring( p_L, 2 );
            //int meshe_index = luaL_checkint( p_L, 3 );
            dsstring meshe_name = luaL_checkstring(p_L, 3);

            /*
            bool status = m_meshe.LoadFromFile( meshe_path, meshe_index );
            if( !status )
            {
                LUA_ERROR( "Body::configure_shape : meshe loading operation failed" );
            }
            */

            dsstring res_id = "body_" + meshe_path;

            m_entity_body_aspect->AddComponent<BodyAspect::MesheCollisionShape>( "shape", m_meshe );
            Meshe* mesheref = &m_entity_body_aspect->GetComponent<BodyAspect::MesheCollisionShape>("shape")->getPurpose().m_meshe;

            m_meshe.SetPath(meshe_path);

            resources_aspect->AddComponent<std::tuple<Meshe*, dsstring, dsstring, bool>>(res_id,
                std::make_tuple(mesheref, meshe_path, meshe_name, false));

        }
        break;    
    }

    return 0;
}

int LuaClass_Body::LUA_configureattitude( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Body::configure_attitude : argument(s) missing" );
	}
    LuaClass_Matrix* lua_mat = Luna<LuaClass_Matrix>::check( p_L, 1 );

    if( NULL == m_entity_body_aspect )
    {
        LUA_ERROR( "Body::configure_attitude : no body aspect" );
    }

    m_entity_body_aspect->AddComponent<Matrix>( "attitude", lua_mat->GetMatrix() );
    m_attitude_setted = true;

    return 0;
}

int LuaClass_Body::LUA_configuremass( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Body::configure_mass : argument(s) missing" );
	}

    dsreal mass = luaL_checknumber( p_L, 1 );

    if( NULL == m_entity_body_aspect )
    {
        LUA_ERROR( "Body::configure_mass : no body aspect" );
    }

    m_entity_body_aspect->AddComponent<dsreal>( "mass", mass );
    m_mass_setted = true;

    return 0;
}

int LuaClass_Body::LUA_configureforce(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 4)
    {
        LUA_ERROR("Body::configure_force : argument(s) missing");
    }

    dsstring forceid = luaL_checkstring(p_L, 1);

    LuaClass_Vector* lua_vec = Luna<LuaClass_Vector>::check(p_L, 2);

    int forcemode = luaL_checkint(p_L, 3);
    bool enabled = luaL_checkint(p_L, 4);

    m_entity_body_aspect->AddComponent<BodyAspect::Force>(forceid, lua_vec->getVector(), (BodyAspect::Force::Mode)forcemode, enabled);
    m_forces_id.push_back(forceid);

    return 0;
}

int LuaClass_Body::LUA_configuretorque(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 4)
    {
        LUA_ERROR("Body::configure_torque : argument(s) missing");
    }

    dsstring torqueid = luaL_checkstring(p_L, 1);

    LuaClass_Vector* lua_vec = Luna<LuaClass_Vector>::check(p_L, 2);

    int torquemode = luaL_checkint(p_L, 3);
    bool enabled = luaL_checkint(p_L, 4);

    m_entity_body_aspect->AddComponent<BodyAspect::Torque>(torqueid, lua_vec->getVector(), (BodyAspect::Torque::Mode)torquemode, enabled);
    m_torques_id.push_back(torqueid);

    return 0;
}

int LuaClass_Body::LUA_configuremode( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Body::configure_mode : argument(s) missing" );
	}

    m_mode = static_cast<DrawSpace::Aspect::BodyAspect::Mode>( luaL_checkint( p_L, 1 ) );

    if( NULL == m_entity_body_aspect )
    {
        LUA_ERROR( "Body::configure_mode : no body aspect" );
    }

    m_entity_body_aspect->AddComponent<BodyAspect::Mode>( "mode", m_mode );

    return 0;
}

int LuaClass_Body::LUA_updateattitude( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Body::update_attitude : argument(s) missing" );
	}
    LuaClass_Matrix* lua_mat = Luna<LuaClass_Matrix>::check( p_L, 1 );

    if( NULL == m_entity_body_aspect )
    {
        LUA_ERROR( "Body::update_attitude : no body aspect" );
    }

    m_entity_body_aspect->GetComponent<Matrix>( "attitude" )->getPurpose() = lua_mat->GetMatrix();

    return 0;
}

int LuaClass_Body::LUA_updateforce(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("Body::update_force : argument(s) missing");
    }

    dsstring forceid = luaL_checkstring(p_L, 1);
    LuaClass_Vector* lua_vec = Luna<LuaClass_Vector>::check(p_L, 2);
    
    m_entity_body_aspect->GetComponent<BodyAspect::Force>(forceid)->getPurpose().UpdateForce(lua_vec->getVector());

    return 0;
}

int LuaClass_Body::LUA_updateforcestate(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("Body::update_forcestate : argument(s) missing");
    }

    dsstring forceid = luaL_checkstring(p_L, 1);
    bool enabled = luaL_checkint(p_L, 2);
    
    if(enabled)
    {
        m_entity_body_aspect->GetComponent<BodyAspect::Force>(forceid)->getPurpose().Enable();
    }
    else
    {
        m_entity_body_aspect->GetComponent<BodyAspect::Force>(forceid)->getPurpose().Disable();
    }

    return 0;
}


int LuaClass_Body::LUA_updatetorque(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("Body::update_torque : argument(s) missing");
    }

    dsstring torqueid = luaL_checkstring(p_L, 1);
    LuaClass_Vector* lua_vec = Luna<LuaClass_Vector>::check(p_L, 2);

    m_entity_body_aspect->GetComponent<BodyAspect::Torque>(torqueid)->getPurpose().UpdateForce(lua_vec->getVector());

    return 0;
}

int LuaClass_Body::LUA_updatetorquestate(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("Body::update_torquestate : argument(s) missing");
    }

    dsstring torqueid = luaL_checkstring(p_L, 1);
    bool enabled = luaL_checkint(p_L, 2);

    if (enabled)
    {
        m_entity_body_aspect->GetComponent<BodyAspect::Torque>(torqueid)->getPurpose().Enable();
    }
    else
    {
        m_entity_body_aspect->GetComponent<BodyAspect::Torque>(torqueid)->getPurpose().Disable();
    }

    return 0;
}


int LuaClass_Body::LUA_zerospeed(lua_State* p_L)
{
    m_entity_body_aspect->GetComponent<bool>("stop_linear_speed")->getPurpose() = true;
    return 0;
}

int LuaClass_Body::LUA_zeroangularspeed(lua_State* p_L)
{
    m_entity_body_aspect->GetComponent<bool>("stop_angular_speed")->getPurpose() = true;
    return 0;
}


int LuaClass_Body::LUA_release( lua_State* p_L )
{
    if( NULL == m_entity_body_aspect )
    {
        LUA_ERROR( "Body::release : no body aspect" );
    }

    if (NULL == m_entity)
    {
        LUA_ERROR("Body::configure_shape : no attached entity");
    }
    ResourcesAspect* resources_aspect = m_entity->GetAspect<ResourcesAspect>();
    if (!resources_aspect)
    {
        LUA_ERROR("Body::configure_shape : attached entity has no resources aspect !");
    }

    if( m_shape_type != -1 )
    {    
       switch( m_shape_type )
       {
            case 0: // SHAPE_BOX
            {                
                m_entity_body_aspect->RemoveComponent<BodyAspect::BoxCollisionShape>( "shape" );
            }
            break;

            case 1: // SHAPE_SPHERE
            {
                m_entity_body_aspect->RemoveComponent<BodyAspect::SphereCollisionShape>( "shape" );
            }
            break;

            case 2: // SHAPE_MESHE
            {
                m_entity_body_aspect->RemoveComponent<BodyAspect::MesheCollisionShape>( "shape" );
                m_meshe.ClearTriangles();
                m_meshe.ClearVertices();

                dsstring meshe_path;
                m_meshe.GetPath(meshe_path);

                dsstring res_id = "body_" + meshe_path;
                resources_aspect->RemoveComponent<std::tuple<Meshe*, dsstring, dsstring, bool>>(res_id);
            }
            break; 
       }

       m_shape_type = -1;
    }

    // release forces & torques
    for(auto& e : m_forces_id)
    {
        m_entity_body_aspect->RemoveComponent<Matrix>(e);
    }
    for (auto& e : m_torques_id)
    {
        m_entity_body_aspect->RemoveComponent<Matrix>(e);
    }


    if( m_attitude_setted )
    {
        m_entity_body_aspect->RemoveComponent<Matrix>( "attitude" );
        m_attitude_setted = false;
    }

    if( m_mass_setted )
    {
        m_entity_body_aspect->RemoveComponent<dsreal>( "mass" );
        m_mass_setted = false;
    }

    if( m_mode != BodyAspect::NOT_READY )
    {
        m_entity_body_aspect->RemoveComponent<BodyAspect::Mode>( "mode" );
        m_mode = BodyAspect::NOT_READY;
    }
    m_entity_body_aspect->Release();

    return 0;
}
