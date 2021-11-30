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
#include "luaclass_freetransform.h"
#include "luaclass_module.h"
#include "luaclass_entity.h"
#include "quaternion.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;


const char LuaClass_FreeTransform::className[] = "FreeTransform";
const Luna<LuaClass_FreeTransform>::RegType LuaClass_FreeTransform::methods[] =
{
    //{ "instanciate_transformimpl", &LuaClass_FreeTransform::LUA_instanciateTransformationImpl },
    //{ "trash_transformimpl", &LuaClass_FreeTransform::LUA_trashTransformationImpl },
    { "configure", &LuaClass_FreeTransform::LUA_configure },
    { "update", &LuaClass_FreeTransform::LUA_update },
	{ "set_pos", &LuaClass_FreeTransform::LUA_setpos },
    { "read", &LuaClass_FreeTransform::LUA_read },
    { "release", &LuaClass_FreeTransform::LUA_release },
	{ 0, 0 }
};

LuaClass_FreeTransform::LuaClass_FreeTransform( lua_State* p_L ) :
m_entity_transform_aspect( NULL )
{
}

LuaClass_FreeTransform::~LuaClass_FreeTransform( void )
{
}

int LuaClass_FreeTransform::LUA_configure( lua_State* p_L )
{
    int argc = lua_gettop(p_L);
    if (argc < 6)
    {
        LUA_ERROR("FreeTransform::configure : argument(s) missing");
    }

    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check(p_L, 1);
    dsreal initial_speed = luaL_checknumber(p_L, 2);
    dsreal initial_posx = luaL_checknumber(p_L, 3);
    dsreal initial_posy = luaL_checknumber(p_L, 4);
    dsreal initial_posz = luaL_checknumber(p_L, 5);
    int transfoimpl_order = luaL_checkint(p_L, 6);

    // extract args

    DrawSpace::Core::Entity& entity = lua_ent->GetEntity();

    // recupere l'aspect transfo s'il existe pour cette entitee
    TransformAspect* transform_aspect = entity.GetAspect<TransformAspect>();
    if (transform_aspect)
    {
        transform_aspect->AddImplementation(transfoimpl_order, this);
        m_entity_transform_aspect = transform_aspect;

        LUA_TRY
        {
            transform_aspect->AddComponent<dsreal>("rspeed_x", 0.0);
            transform_aspect->AddComponent<dsreal>("rspeed_y", 0.0);
            transform_aspect->AddComponent<dsreal>("rspeed_z", 0.0);

            transform_aspect->AddComponent<Vector>("speed", Vector(0.0, 0.0, initial_speed, 0.0 ));
            transform_aspect->AddComponent<Vector>("rot_axis_x", Vector(1.0, 0.0, 0.0, 1.0));
            transform_aspect->AddComponent<Vector>("rot_axis_y", Vector(0.0, 1.0, 0.0, 1.0));
            transform_aspect->AddComponent<Vector>("rot_axis_z", Vector(0.0, 0.0, 1.0, 1.0));

            transform_aspect->AddComponent<Matrix>("pos");
            transform_aspect->GetComponent<Matrix>("pos")->getPurpose().Translation(Vector(initial_posx, initial_posy, initial_posz, 1.0));

            transform_aspect->AddComponent<Quaternion>("quat");
            transform_aspect->GetComponent<Quaternion>("quat")->getPurpose().Identity();

        } LUA_CATCH;
    }
    else
    {
        m_entity_transform_aspect = NULL;
        LUA_ERROR("FreeTransform::configure : entity passed on arg has no transform aspect");
    }

    return 0;
}

int LuaClass_FreeTransform::LUA_release( lua_State* p_L )
{
    if (m_entity_transform_aspect)
    {
        LUA_TRY
        {
            m_entity_transform_aspect->RemoveComponent<dsreal>("rspeed_x");
            m_entity_transform_aspect->RemoveComponent<dsreal>("rspeed_y");
            m_entity_transform_aspect->RemoveComponent<dsreal>("rspeed_z");

            m_entity_transform_aspect->RemoveComponent<Vector>("speed");
            m_entity_transform_aspect->RemoveComponent<Vector>("rot_axis_x");
            m_entity_transform_aspect->RemoveComponent<Vector>("rot_axis_y");
            m_entity_transform_aspect->RemoveComponent<Vector>("rot_axis_z");
            m_entity_transform_aspect->RemoveComponent<Matrix>("pos");
            m_entity_transform_aspect->RemoveComponent<Quaternion>("quat");

            //m_entity_transform_aspect->RemoveImplementation();
            m_entity_transform_aspect->RemoveAllImplementations();

        } LUA_CATCH;

        m_entity_transform_aspect = NULL;
    }
    else
    {
        LUA_ERROR("FreeTransform::release : no transform aspect");
    }
    return 0;
}

int LuaClass_FreeTransform::LUA_update( lua_State* p_L )
{
    if (!m_entity_transform_aspect)
    {
        LUA_ERROR("FreeTransform::update : no transform aspect");
    }

    int argc = lua_gettop(p_L);
    if (argc < 7)
    {
        LUA_ERROR("FreeTransform::update : argument(s) missing");
    }

    dsreal speed = luaL_checknumber(p_L, 1);
    dsreal posx = luaL_checknumber(p_L, 2);
    dsreal posy = luaL_checknumber(p_L, 3);
    dsreal posz = luaL_checknumber(p_L, 4);

    dsreal rotx_speed = luaL_checknumber(p_L, 5);
    dsreal roty_speed = luaL_checknumber(p_L, 6);
    dsreal rotz_speed = luaL_checknumber(p_L, 7);

    LUA_TRY
    {
        m_entity_transform_aspect->GetComponent<Vector>("speed")->getPurpose() = Vector(0.0, 0.0, speed, 0.0);

        m_entity_transform_aspect->GetComponent<dsreal>("rspeed_x")->getPurpose() = rotx_speed;
        m_entity_transform_aspect->GetComponent<dsreal>("rspeed_y")->getPurpose() = roty_speed;
        m_entity_transform_aspect->GetComponent<dsreal>("rspeed_z")->getPurpose() = rotz_speed;

        m_entity_transform_aspect->GetComponent<Matrix>("pos")->getPurpose().Translation(Vector(posx, posy, posz, 1.0));

    } LUA_CATCH;

    return 0;
}

int LuaClass_FreeTransform::LUA_setpos(lua_State* p_L)
{
	if (!m_entity_transform_aspect)
	{
		LUA_ERROR("FreeTransform::set_pos : no transform aspect");
	}

	int argc = lua_gettop(p_L);
	if (argc < 3)
	{
		LUA_ERROR("FreeTransform::set_pos : argument(s) missing");
	}

	dsreal posx = luaL_checknumber(p_L, 1);
	dsreal posy = luaL_checknumber(p_L, 2);
	dsreal posz = luaL_checknumber(p_L, 3);

	LUA_TRY
	{
		m_entity_transform_aspect->GetComponent<Matrix>("pos")->getPurpose().Translation(Vector(posx, posy, posz, 1.0));

	} LUA_CATCH;

	return 0;
}

int LuaClass_FreeTransform::LUA_read( lua_State* p_L )
{
    if (!m_entity_transform_aspect)
    {
        LUA_ERROR("FreeTransform::read : no transform aspect");
    }

    LUA_TRY
    {
        dsreal speed = m_entity_transform_aspect->GetComponent<Vector>("speed")->getPurpose()[2];
        Matrix mat = m_entity_transform_aspect->GetComponent<Matrix>("pos")->getPurpose();

        dsreal posx = mat(3, 0);
        dsreal posy = mat(3, 1);
        dsreal posz = mat(3, 2);

        lua_pushnumber(p_L, posx);
        lua_pushnumber(p_L, posy);
        lua_pushnumber(p_L, posz);
        lua_pushnumber(p_L, speed);


    } LUA_CATCH;
    return 4;
}

void LuaClass_FreeTransform::GetLocaleTransform(DrawSpace::Aspect::TransformAspect* p_transformaspect, DrawSpace::Utils::Matrix& p_out_base_transform)
{
    if (NULL == m_time_aspect)
    {
        _DSEXCEPTION("Need a time reference!!!")
    }

    // recup des composants donnees d'entr�es

    ComponentList<Vector> vectors;
    p_transformaspect->GetComponentsByType<Vector>(vectors);

    Vector local_speed = vectors[0]->getPurpose();
    local_speed[2] = -local_speed[2];

    ComponentList<Matrix> mats;
    p_transformaspect->GetComponentsByType<Matrix>(mats);

    Matrix pos = mats[0]->getPurpose();

    ComponentList<TimeAspect::TimeScale> time_scales;
    m_time_aspect->GetComponentsByType<TimeAspect::TimeScale>(time_scales);


    //vitesses demand�es...

    ComponentList<dsreal> reals;
    p_transformaspect->GetComponentsByType<dsreal>(reals);
    dsreal rspeed_x = reals[0]->getPurpose();
    dsreal rspeed_y = reals[1]->getPurpose();
    dsreal rspeed_z = reals[2]->getPurpose();

    if (TimeAspect::FREEZE == time_scales[0]->getPurpose())
    {
        rspeed_x = rspeed_y = rspeed_z = 0.0;
    }


    // axe demand�
    Vector rot_axis_x = vectors[1]->getPurpose();
    Vector rot_axis_y = vectors[2]->getPurpose();
    Vector rot_axis_z = vectors[3]->getPurpose();

    // quaternion resultat courant
    ComponentList<Quaternion>   quats;
    p_transformaspect->GetComponentsByType<Quaternion>(quats);
    Quaternion	                current_res = quats[0]->getPurpose();

    Utils::Matrix			    orientation;
    Vector                      gs;

    ////////////////////////////////////////


    Quaternion q, qres;
    dsreal fps;


    /// NB: l'ordre dans lequel sont traites les axes n'a pas d'importance...

    /////////////////Axe X /////////////////////////////

    //fps = tm->ConvertUnitPerSecFramePerSec( rspeed_x );
    fps = m_time_aspect->ConvertUnitPerSecFramePerSec(rspeed_x);
    q.RotationAxis(rot_axis_x, fps);
    qres = current_res * q;
    current_res = qres;

    current_res.RotationMatFrom(orientation);
    rot_axis_x[0] = orientation(0, 0);
    rot_axis_x[1] = orientation(0, 1);
    rot_axis_x[2] = orientation(0, 2);

    /////////////////Axe Y /////////////////////////////

    //fps = tm->ConvertUnitPerSecFramePerSec( rspeed_y );
    fps = m_time_aspect->ConvertUnitPerSecFramePerSec(rspeed_y);
    q.RotationAxis(rot_axis_y, fps);
    qres = current_res * q;
    current_res = qres;

    current_res.RotationMatFrom(orientation);
    rot_axis_y[0] = orientation(1, 0);
    rot_axis_y[1] = orientation(1, 1);
    rot_axis_y[2] = orientation(1, 2);

    /////////////////Axe Z /////////////////////////////

    //fps = tm->ConvertUnitPerSecFramePerSec( rspeed_z );
    fps = m_time_aspect->ConvertUnitPerSecFramePerSec(rspeed_z);
    q.RotationAxis(rot_axis_z, fps);
    qres = current_res * q;
    current_res = qres;

    current_res.RotationMatFrom(orientation);
    rot_axis_z[0] = orientation(2, 0);
    rot_axis_z[1] = orientation(2, 1);
    rot_axis_z[2] = orientation(2, 2);


    // resultat calcul est dans 'orientation'

    ////////////////////////////////////////

    orientation.Transform(&local_speed, &gs);

    TimeAspect::TimeScalar pos_30 = m_time_aspect->TimeScalarFactory(pos(3, 0));
    TimeAspect::TimeScalar pos_31 = m_time_aspect->TimeScalarFactory(pos(3, 1));
    TimeAspect::TimeScalar pos_32 = m_time_aspect->TimeScalarFactory(pos(3, 2));

    pos_30 += gs[0];
    pos_31 += gs[1];
    pos_32 += gs[2];

    pos(3, 0) = pos_30.GetValue();
    pos(3, 1) = pos_31.GetValue();
    pos(3, 2) = pos_32.GetValue();

    p_out_base_transform = orientation * pos;

    mats[0]->getPurpose() = pos;
    quats[0]->getPurpose() = current_res;

    vectors[1]->getPurpose() = rot_axis_x;
    vectors[2]->getPurpose() = rot_axis_y;
    vectors[3]->getPurpose() = rot_axis_z;
}