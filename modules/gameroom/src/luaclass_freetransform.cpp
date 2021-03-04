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
    { "instanciate_transformimpl", &LuaClass_FreeTransform::LUA_instanciateTransformationImpl },
    { "trash_transformimpl", &LuaClass_FreeTransform::LUA_trashTransformationImpl },
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

int LuaClass_FreeTransform::LUA_instanciateTransformationImpl(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("FreeTransform::instanciate_transformimpl : argument(s) missing");
    }
    LuaClass_Module* lua_mod = Luna<LuaClass_Module>::check(p_L, 1);

    m_free_transformer = lua_mod->GetModuleRoot()->InstanciateTransformAspectImpls("free");
    return 0;
}

int LuaClass_FreeTransform::LUA_trashTransformationImpl(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("FreeTransform::trash_transformimpl : argument(s) missing");
    }
    LuaClass_Module* lua_mod = Luna<LuaClass_Module>::check(p_L, 1);

    lua_mod->GetModuleRoot()->TrashTransformAspectImpls(m_free_transformer);
    return 0;
}

int LuaClass_FreeTransform::LUA_configure( lua_State* p_L )
{
    int argc = lua_gettop(p_L);
    if (argc < 5)
    {
        LUA_ERROR("FreeTransform::configure : argument(s) missing");
    }

    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check(p_L, 1);
    dsreal initial_speed = luaL_checknumber(p_L, 2);
    dsreal initial_posx = luaL_checknumber(p_L, 3);
    dsreal initial_posy = luaL_checknumber(p_L, 4);
    dsreal initial_posz = luaL_checknumber(p_L, 5);


    // extract args

    DrawSpace::Core::Entity& entity = lua_ent->GetEntity();

    // recupere l'aspect transfo s'il existe pour cette entitee
    TransformAspect* transform_aspect = entity.GetAspect<TransformAspect>();
    if (transform_aspect)
    {
        transform_aspect->AddImplementation(m_free_transformer);
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
