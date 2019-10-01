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

#include "luacontext.h"
#include "luaclass_entity.h"
#include "luaclass_renderpassnodegraph.h"
#include "luaclass_matrix.h"
#include "renderingaspect.h"
#include "timeaspect.h"

#include "bodyaspect.h"
#include "cameraaspect.h"
#include "physicsaspect.h"
#include "renderingaspect.h"
#include "serviceaspect.h"
#include "informationsaspect.h"
#include "timeaspect.h"
#include "transformaspect.h"
#include "resourcesaspect.h"
#include "animationsaspect.h"
#include "textrenderingaspectimpl.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;

const char LuaClass_Entity::className[] = "Entity";
const Luna<LuaClass_Entity>::RegType LuaClass_Entity::methods[] =
{
    { "add_aspect", &LuaClass_Entity::LUA_addaspect },
    { "remove_aspect", &LuaClass_Entity::LUA_removeaspect },
    { "connect_renderingaspect_rendergraph", &LuaClass_Entity::LUA_connect_renderingaspect_rendergraph },
    { "configure_timemanager", &LuaClass_Entity::LUA_configuretimemmanager },
    { "read_timemanager", &LuaClass_Entity::LUA_readtimemmanager },
    { "update_timescale", &LuaClass_Entity::LUA_updatetimescale },
    { "update_time", &LuaClass_Entity::LUA_updatetime },
    { "release_timemanager", &LuaClass_Entity::LUA_releasetimemmanager },
    { "configure_world", &LuaClass_Entity::LUA_configureworld },
    { "release_world", &LuaClass_Entity::LUA_releaseworld },
    { "configure_camera", &LuaClass_Entity::LUA_configurecamera },
    { "release_camera", &LuaClass_Entity::LUA_releasecamera },
	{ "configure_animationbones", &LuaClass_Entity::LUA_configureanimationbones },
	{ "update_bonelocaltransform", &LuaClass_Entity::LUA_updatebonelocaltransform },
	{ "read_currentanimationinfos", &LuaClass_Entity::LUA_readcurrentanimationinfos },
	{ "read_animationsnames", &LuaClass_Entity::LUA_readanimationsnames },
	{ "set_currentanimation", &LuaClass_Entity::LUA_setcurrentanimation },
	{ "release_animationbones", &LuaClass_Entity::LUA_releaseanimationbones },
    { "setup_info", &LuaClass_Entity::LUA_setupinfo },
    { "release_info", &LuaClass_Entity::LUA_releaseinfo },
	{ 0, 0 }
};

LuaClass_Entity::LuaClass_Entity( lua_State* p_L )
{
}

LuaClass_Entity::~LuaClass_Entity( void )
{
}


DrawSpace::Core::Entity& LuaClass_Entity::GetEntity( void )
{
    return m_entity;
}

int LuaClass_Entity::LUA_addaspect( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Entity::add_aspect : argument(s) missing" );
	}

    AspectType aspect_type = static_cast<AspectType>( luaL_checkint( p_L, 1 ) );

    static const std::map<AspectType, std::function<void( DrawSpace::Core::Entity& )>> aspect_add_aig = 
    {
        { BODY_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<BodyAspect>(); } },
        { CAMERA_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<CameraAspect>(); } },
        { PHYSICS_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<PhysicsAspect>(); } },
        { RENDERING_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<RenderingAspect>(); } },
        { SERVICE_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<ServiceAspect>(); } },
        { TIME_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<TimeAspect>(); } },
        { TRANSFORM_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<TransformAspect>(); } },
        { INFOS_ASPECT, [](DrawSpace::Core::Entity& p_entity) { p_entity.AddAspect<InfosAspect>(); } },
        { RESOURCES_ASPECT, [](DrawSpace::Core::Entity& p_entity) { p_entity.AddAspect<ResourcesAspect>(); } },
		{ ANIMATION_ASPECT, [](DrawSpace::Core::Entity& p_entity) { p_entity.AddAspect<AnimationsAspect>(); } },
    };

    aspect_add_aig.at(aspect_type)( m_entity );
    return 0;
}

int LuaClass_Entity::LUA_removeaspect( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Entity::remove_aspect : argument(s) missing" );
	}
    AspectType aspect_type = static_cast<AspectType>( luaL_checkint( p_L, 1 ) );

    static const std::map<AspectType, std::function<void( DrawSpace::Core::Entity& )>> aspect_remove_aig = 
    {
        { BODY_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<BodyAspect>(); } },
        { CAMERA_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<CameraAspect>(); } },
        { PHYSICS_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<PhysicsAspect>(); } },
        { RENDERING_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<RenderingAspect>(); } },
        { SERVICE_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<ServiceAspect>(); } },
        { TIME_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<TimeAspect>(); } },
        { TRANSFORM_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<TransformAspect>(); } },
        { INFOS_ASPECT, [](DrawSpace::Core::Entity& p_entity) { p_entity.RemoveAspect<InfosAspect>(); } },
        { RESOURCES_ASPECT, [](DrawSpace::Core::Entity& p_entity) { p_entity.RemoveAspect<ResourcesAspect>(); } },
		{ ANIMATION_ASPECT, [](DrawSpace::Core::Entity& p_entity) { p_entity.RemoveAspect<AnimationsAspect>(); } },
    };

    aspect_remove_aig.at(aspect_type)( m_entity );
    return 0;
}

int LuaClass_Entity::LUA_configuretimemmanager( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Entity::configure_timemanager : argument(s) missing" );
	}

    TimeAspect* time_aspect = m_entity.GetAspect<TimeAspect>();
    if( NULL == time_aspect )
    {
        LUA_ERROR( "Entity::configure_timemanager : time aspect doesnt exists in this entity!" );
    }

    int time_scale = luaL_checkint( p_L, 1 );

    LUA_TRY
    {
        time_aspect->AddComponent<TimeManager>( "time_manager" );
        time_aspect->AddComponent<TimeAspect::TimeScale>( "time_scale", static_cast<TimeAspect::TimeScale>( time_scale ) );
        time_aspect->AddComponent<dsstring>( "output_formated_datetime", "..." );
        time_aspect->AddComponent<dstime>( "time", 0 );
        time_aspect->AddComponent<int>( "output_fps" );
        time_aspect->AddComponent<int>( "output_world_nbsteps" );

        time_aspect->AddComponent<dsreal>( "output_time_factor" );

    } LUA_CATCH;

    return 0;
}

int LuaClass_Entity::LUA_readtimemmanager( lua_State* p_L )
{
    TimeAspect* time_aspect = m_entity.GetAspect<TimeAspect>();
    if( NULL == time_aspect )
    {
        LUA_ERROR( "Entity::read_timemanager : time aspect doesnt exists in this entity!" );
    }

    LUA_TRY
    {
        TimeAspect::TimeScale time_scale = time_aspect->GetComponent<TimeAspect::TimeScale>( "time_scale" )->getPurpose();
        dsstring datetime = time_aspect->GetComponent<dsstring>( "output_formated_datetime" )->getPurpose();
        int fps = time_aspect->GetComponent<int>( "output_fps" )->getPurpose();
        int world_nbsteps = time_aspect->GetComponent<int>( "output_world_nbsteps" )->getPurpose();

        lua_pushinteger( p_L, time_scale );
        lua_pushstring( p_L, datetime.c_str() );
        lua_pushinteger( p_L, fps );
        lua_pushinteger( p_L, world_nbsteps );

    } LUA_CATCH;

    return 4;
}

int LuaClass_Entity::LUA_updatetimescale( lua_State* p_L )
{
    TimeAspect* time_aspect = m_entity.GetAspect<TimeAspect>();
    if( NULL == time_aspect )
    {
        LUA_ERROR( "Entity::update_timescale : time aspect doesnt exists in this entity!" );
    }

	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Entity::update_timescale : argument(s) missing" );
	}

    TimeAspect::TimeScale time_scale = static_cast<TimeAspect::TimeScale>( luaL_checkint( p_L, 1 ) );
    LUA_TRY
    {
        time_aspect->GetComponent<TimeAspect::TimeScale>( "time_scale" )->getPurpose() = time_scale;

    } LUA_CATCH;
    return 0;
}

int LuaClass_Entity::LUA_updatetime(lua_State* p_L)
{
    TimeAspect* time_aspect = m_entity.GetAspect<TimeAspect>();
    if (NULL == time_aspect)
    {
        LUA_ERROR("Entity::update_time : time aspect doesnt exists in this entity!");
    }

    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Entity::update_time : argument(s) missing");
    }

    dstime yearval = (dstime)luaL_checkint(p_L, 1);

    LUA_TRY
    {
        dstime time = yearval * (dstime)31536000;
        time_aspect->GetComponent<dstime>("time")->getPurpose() = time;

    } LUA_CATCH;
    return 0;
}


int LuaClass_Entity::LUA_releasetimemmanager( lua_State* p_L )
{
    TimeAspect* time_aspect = m_entity.GetAspect<TimeAspect>();
    if( NULL == time_aspect )
    {
        LUA_ERROR( "Entity::release_timemanager : time aspect doesnt exists in this entity!" );
    }

    LUA_TRY
    {
        time_aspect->RemoveComponent<TimeManager>( "time_manager" );
        time_aspect->RemoveComponent<TimeAspect::TimeScale>( "time_scale" );
        time_aspect->RemoveComponent<dsstring>( "output_formated_datetime" );
        time_aspect->RemoveComponent<dstime>( "time" );
        time_aspect->RemoveComponent<int>( "output_fps" );
        time_aspect->RemoveComponent<int>( "output_world_nbsteps" );
        time_aspect->RemoveComponent<dsreal>( "output_time_factor" );

    } LUA_CATCH;
    return 0;
}

int LuaClass_Entity::LUA_configureworld( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Entity::configure_world : argument(s) missing" );
	}

    bool gravity = luaL_checkint( p_L, 1 );

    if( gravity && argc < 4 )
    {
        LUA_ERROR( "Entity::configure_world : argument(s) missing" );
    }
    dsreal xg;
    dsreal yg;
    dsreal zg;

    if( argc == 4 )
    {
        xg = luaL_checknumber( p_L, 2 );
        yg = luaL_checknumber( p_L, 3 );
        zg = luaL_checknumber( p_L, 4 );
    }

    PhysicsAspect* physics_aspect = m_entity.GetAspect<PhysicsAspect>();
    if( NULL == physics_aspect )
    {
        LUA_ERROR( "Entity::configure_world : physics aspect doesnt exists in this entity!" );
    }

    physics_aspect->AddComponent<bool>( "gravity_state", gravity );

    if( gravity )
    {
        physics_aspect->AddComponent<Vector>( "gravity", Vector( xg, yg, zg, 1.0 ) );
    }

    return 0;
}

int LuaClass_Entity::LUA_releaseworld( lua_State* p_L )
{
    PhysicsAspect* physics_aspect = m_entity.GetAspect<PhysicsAspect>();
    if( NULL == physics_aspect )
    {
        LUA_ERROR( "Entity::release_world : physics aspect doesnt exists in this entity!" );
    }

    if( physics_aspect->GetComponent<bool>( "gravity_state" )->getPurpose() )
    {
        physics_aspect->RemoveComponent<Vector>( "gravity" );
    }

    physics_aspect->RemoveComponent<bool>( "gravity_state" );

    return 0;
}


int LuaClass_Entity::LUA_configurecamera( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 4 )
	{
        LUA_ERROR( "Entity::configure_camera : argument(s) missing" );
	}

    CameraAspect* camera_aspect = m_entity.GetAspect<CameraAspect>();
    if( NULL == camera_aspect )
    {
        LUA_ERROR( "Entity::configure_camera : camera_aspect aspect doesnt exists in this entity!" );
    }

    dsreal w = luaL_checknumber( p_L, 1 );
    dsreal h = luaL_checknumber( p_L, 2 );
    dsreal zn = luaL_checknumber( p_L, 3 );
    dsreal zf = luaL_checknumber( p_L, 4 );

    dsstring camera_name = "camera";

    if( argc == 5 )
    {
        camera_name = luaL_checkstring(p_L, 5);
    }

    Matrix proj;
    proj.Perspective( w, h, zn, zf );
    camera_aspect->AddComponent<Matrix>( "camera_proj", proj );

    camera_aspect->AddComponent<dsstring>( "camera_name", camera_name );
    return 0;
}

int LuaClass_Entity::LUA_releasecamera( lua_State* p_L )
{
    CameraAspect* camera_aspect = m_entity.GetAspect<CameraAspect>();
    if( NULL == camera_aspect )
    {
        LUA_ERROR( "Entity::release_camera : camera aspect doesnt exists in this entity!" );
    }

    LUA_TRY
    {
        camera_aspect->RemoveComponent<Matrix>( "camera_proj" );
        camera_aspect->RemoveComponent<dsstring>("camera_name");

    } LUA_CATCH;

    return 0;
}

int LuaClass_Entity::LUA_configureanimationbones(lua_State* p_L)
{
	AnimationsAspect* animation_aspect = m_entity.GetAspect<AnimationsAspect>();
	if (NULL == animation_aspect)
	{
		LUA_ERROR("Entity::configure_animationbones : animation aspect doesnt exists in this entity!");
	}

	LUA_TRY
	{
		animation_aspect->AddComponent<std::map<dsstring, AnimationsAspect::Node>>("nodes");
		animation_aspect->AddComponent<std::map<dsstring, int>>("bones_mapping");
		animation_aspect->AddComponent<std::vector<AnimationsAspect::BoneOutput>>("bones_outputs");
		animation_aspect->AddComponent<dsstring>("nodes_root_id");

		animation_aspect->AddComponent<std::map<dsstring, Matrix>>("forced_bones_transformations");

		animation_aspect->AddComponent<std::map<dsstring, AnimationsAspect::AnimationRoot>>("animations");

		animation_aspect->AddComponent<dsstring>("current_animation_name");
		animation_aspect->AddComponent<long>("current_animation_ticks_per_seconds");
		animation_aspect->AddComponent<dsreal>("current_animation_ticks_progress");
		animation_aspect->AddComponent<dsreal>("current_animation_seconds_progress");

		animation_aspect->AddComponent<dsreal>("current_animation_ticks_duration");
		animation_aspect->AddComponent<dsreal>("current_animation_seconds_duration");
		
		animation_aspect->AddComponent<TimeAspect::TimeMark>("current_animation_timemark");


	} LUA_CATCH;

	return 0;
}

int LuaClass_Entity::LUA_updatebonelocaltransform(lua_State* p_L)
{
	AnimationsAspect* animation_aspect = m_entity.GetAspect<AnimationsAspect>();
	if (NULL == animation_aspect)
	{
		LUA_ERROR("Entity::update_bonelocaltransform : animation aspect doesnt exists in this entity!");
	}

	int argc = lua_gettop(p_L);
	if (argc < 2)
	{
		LUA_ERROR("Entity::update_bonelocaltransform : argument(s) missing");
	}

	dsstring bone_id = luaL_checkstring(p_L, 1);
	LuaClass_Matrix* mat = Luna<LuaClass_Matrix>::check(p_L, 2);

	auto& forced_pos = animation_aspect->GetComponent<std::map<dsstring, Matrix>>("forced_bones_transformations")->getPurpose();

	forced_pos[bone_id] = mat->GetMatrix();

	return 0;
}

int LuaClass_Entity::LUA_readcurrentanimationinfos(lua_State* p_L)
{
	AnimationsAspect* animation_aspect = m_entity.GetAspect<AnimationsAspect>();
	if (NULL == animation_aspect)
	{
		LUA_ERROR("Entity::read_currentanimationinfos : animation aspect doesnt exists in this entity!");
	}

	lua_pushstring(p_L, animation_aspect->GetComponent<dsstring>("current_animation_name")->getPurpose().c_str());
	lua_pushinteger(p_L, animation_aspect->GetComponent<long>("current_animation_ticks_per_seconds")->getPurpose());
	lua_pushnumber(p_L, animation_aspect->GetComponent<dsreal>("current_animation_ticks_duration")->getPurpose());
	lua_pushnumber(p_L, animation_aspect->GetComponent<dsreal>("current_animation_seconds_duration")->getPurpose());
	lua_pushnumber(p_L, animation_aspect->GetComponent<dsreal>("current_animation_ticks_progress")->getPurpose());
	lua_pushnumber(p_L, animation_aspect->GetComponent<dsreal>("current_animation_seconds_progress")->getPurpose());

	return 6;
}

int LuaClass_Entity::LuaClass_Entity::LUA_readanimationsnames(lua_State* p_L)
{
	AnimationsAspect* animation_aspect = m_entity.GetAspect<AnimationsAspect>();
	if (NULL == animation_aspect)
	{
		LUA_ERROR("Entity::read_animationsnames : animation aspect doesnt exists in this entity!");
	}

	auto animations_table = animation_aspect->GetComponent<std::map<dsstring, AnimationsAspect::AnimationRoot>>("animations")->getPurpose();

	for (auto& e : animations_table)
	{
		lua_pushstring(p_L, e.first.c_str());
	}

	return animations_table.size();
}

int LuaClass_Entity::LUA_setcurrentanimation(lua_State* p_L)
{
	int argc = lua_gettop(p_L);
	if (argc < 1)
	{
		LUA_ERROR("Entity::set_currentanimation : argument(s) missing");
	}

	dsstring animation_name = luaL_checkstring(p_L, 1);

	AnimationsAspect* animation_aspect = m_entity.GetAspect<AnimationsAspect>();
	if (NULL == animation_aspect)
	{
		LUA_ERROR("Entity::set_currentanimation : animation aspect doesnt exists in this entity!");
	}

	TransformAspect* transform_aspect = m_entity.GetAspect<TransformAspect>();
	if (NULL == transform_aspect)
	{
		LUA_ERROR("Entity::configure_animationbones : transformation aspect doesnt exists in this entity!");
	}
	TimeAspect* time_aspect = transform_aspect->GetTimeAspectRef();

	auto animations_table = animation_aspect->GetComponent<std::map<dsstring, AnimationsAspect::AnimationRoot>>("animations")->getPurpose();

	if (animations_table.find(animation_name) == animations_table.end())
	{
		LUA_ERROR("Entity::set_currentanimation : unknown animation name!");
	}
	else
	{
		if ("" == animation_aspect->GetComponent<dsstring>("current_animation_name")->getPurpose())
		{
			animation_aspect->GetComponent<dsstring>("current_animation_name")->getPurpose() = animation_name;
			animation_aspect->GetComponent<long>("current_animation_ticks_per_seconds")->getPurpose() = animations_table.at(animation_name).ticksPerSeconds;

			animation_aspect->GetComponent<dsreal>("current_animation_ticks_progress")->getPurpose() = 0;
			animation_aspect->GetComponent<dsreal>("current_animation_seconds_progress")->getPurpose() = 0;

			animation_aspect->GetComponent<dsreal>("current_animation_ticks_duration")->getPurpose() = animations_table.at(animation_name).duration;
			animation_aspect->GetComponent<dsreal>("current_animation_seconds_duration")->getPurpose() = animations_table.at(animation_name).duration / animations_table.at(animation_name).ticksPerSeconds;

			animation_aspect->GetComponent<TimeAspect::TimeMark>("current_animation_timemark")->getPurpose() = time_aspect->TimeMarkFactory();

			animation_aspect->GetComponent<TimeAspect::TimeMark>("current_animation_timemark")->getPurpose().Reset();
		}
		else
		{
			LUA_ERROR("Entity::set_currentanimation : another animation currently running!");
		}
	}
	return 0;
}

int LuaClass_Entity::LUA_releaseanimationbones(lua_State* p_L)
{
	AnimationsAspect* animation_aspect = m_entity.GetAspect<AnimationsAspect>();
	if (NULL == animation_aspect)
	{
		LUA_ERROR("Entity::configure_animationbones : animation aspect doesnt exists in this entity!");
	}

	LUA_TRY
	{
		animation_aspect->RemoveComponent<std::map<dsstring, AnimationsAspect::Node>>("nodes");
		animation_aspect->RemoveComponent<std::map<dsstring, int>>("bones_mapping");
		animation_aspect->RemoveComponent<std::vector<AnimationsAspect::BoneOutput>>("bones_outputs");
		animation_aspect->RemoveComponent<dsstring>("nodes_root_id");

		animation_aspect->RemoveComponent<std::map<dsstring, Matrix>>("forced_bones_transformations");

		animation_aspect->RemoveComponent<std::map<dsstring, AnimationsAspect::AnimationRoot>>("animations");

		animation_aspect->RemoveComponent<dsstring>("current_animation_name");
		animation_aspect->RemoveComponent<long>("current_animation_ticks_per_seconds");

		animation_aspect->RemoveComponent<dsreal>("current_animation_ticks_progress");
		animation_aspect->RemoveComponent<dsreal>("current_animation_seconds_progress");

		animation_aspect->RemoveComponent<dsreal>("current_animation_ticks_duration");
		animation_aspect->RemoveComponent<dsreal>("current_animation_seconds_duration");

		animation_aspect->RemoveComponent<TimeAspect::TimeMark>("current_animation_timemark");

	} LUA_CATCH;

	return 0;
}


int LuaClass_Entity::LUA_setupinfo(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("Entity::setup_infos : argument(s) missing");
    }

    InfosAspect* infos_aspect = m_entity.GetAspect<InfosAspect>();
    if (NULL == infos_aspect)
    {
        LUA_ERROR("Entity::setup_infos : infos aspect doesnt exists in this entity!");
    }

	LUA_TRY
	{
		dsstring key_string = luaL_checkstring(p_L, 1);
		dsstring infos_string = luaL_checkstring(p_L, 2);
		infos_aspect->AddComponent<dsstring>(key_string, infos_string);

	} LUA_CATCH;

    return 0;
}

int LuaClass_Entity::LUA_releaseinfo(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Entity::release_infos : argument(s) missing");
    }

    InfosAspect* infos_aspect = m_entity.GetAspect<InfosAspect>();
    if (NULL == infos_aspect)
    {
        LUA_ERROR("Entity::release_infos : infos aspect doesnt exists in this entity!");
    }

    dsstring key_string = luaL_checkstring(p_L, 1);

    LUA_TRY
    {
        infos_aspect->RemoveComponent<dsstring>(key_string);

    } LUA_CATCH;

    return 0;
}

int LuaClass_Entity::LUA_connect_renderingaspect_rendergraph( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "Entity::connect_renderingaspect_rendergraph : argument(s) missing" );
	}

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check( p_L, 1 );
	LUA_TRY
	{
		RenderingAspect* rendering_aspect = m_entity.GetAspect<RenderingAspect>();
		if( rendering_aspect )
		{
			rendering_aspect->AddImplementation( &lua_rg->GetPassesRenderAspectImpl(), NULL );
		}
		else
		{
			LUA_ERROR( "Entity::connect_renderingaspect_rendergraph : entity has no rendering aspect. Call add_renderingaspect() method first" );
		}

	} LUA_CATCH;

    return 0;
}
