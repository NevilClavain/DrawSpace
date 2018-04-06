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

#include "luacontext.h"
#include "luaclass_entity.h"
#include "luaclass_renderpassnodegraph.h"
#include "renderingaspect.h"
#include "timeaspect.h"

#include "bodyaspect.h"
#include "cameraaspect.h"
#include "physicsaspect.h"
#include "renderingaspect.h"
#include "serviceaspect.h"
#include "timeaspect.h"
#include "transformaspect.h"
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
    { "release_timemanager", &LuaClass_Entity::LUA_releasetimemmanager },
    { "configure_camera", &LuaClass_Entity::LUA_configurecamera },
    { "release_camera", &LuaClass_Entity::LUA_releasecamera },
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

    static std::map<AspectType, std::function<void( DrawSpace::Core::Entity& )>> aspect_add_aig = 
    {
        { BODY_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<BodyAspect>(); } },
        { CAMERA_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<CameraAspect>(); } },
        { PHYSICS_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<PhysicsAspect>(); } },
        { RENDERING_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<RenderingAspect>(); } },
        { SERVICE_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<ServiceAspect>(); } },
        { TIME_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<TimeAspect>(); } },
        { TRANSFORM_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.AddAspect<TransformAspect>(); } },
    };

    aspect_add_aig[aspect_type]( m_entity );
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

    static std::map<AspectType, std::function<void( DrawSpace::Core::Entity& )>> aspect_remove_aig = 
    {
        { BODY_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<BodyAspect>(); } },
        { CAMERA_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<CameraAspect>(); } },
        { PHYSICS_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<PhysicsAspect>(); } },
        { RENDERING_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<RenderingAspect>(); } },
        { SERVICE_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<ServiceAspect>(); } },
        { TIME_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<TimeAspect>(); } },
        { TRANSFORM_ASPECT, []( DrawSpace::Core::Entity& p_entity ) { p_entity.RemoveAspect<TransformAspect>(); } },
    };

    aspect_remove_aig[aspect_type]( m_entity );
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

int LuaClass_Entity::LUA_configurecamera( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
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

    Matrix proj;
    proj.Perspective( w, h, zn, zf );
    camera_aspect->AddComponent<Matrix>( "camera_proj", proj );

    return 0;
}

int LuaClass_Entity::LUA_releasecamera( lua_State* p_L )
{
    CameraAspect* camera_aspect = m_entity.GetAspect<CameraAspect>();
    if( NULL == camera_aspect )
    {
        LUA_ERROR( "Entity::release_camera : camera_aspect aspect doesnt exists in this entity!" );
    }

    LUA_TRY
    {
        camera_aspect->RemoveComponent<Matrix>( "camera_proj" );

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
    if( NULL == lua_rg )
    {
        LUA_ERROR( "Entity::connect_renderingaspect_rendergraph : arg 1 must be of type LuaClass_RenderPassNodeGraph" );
    }

    RenderingAspect* rendering_aspect = m_entity.GetAspect<RenderingAspect>();
    if( rendering_aspect )
    {
        rendering_aspect->AddImplementation( &lua_rg->GetPassesRenderAspectImpl() );
    }
    else
    {
        LUA_ERROR( "Entity::connect_renderingaspect_rendergraph : entity has no rendering aspect. Call add_renderingaspect() method first" );
    }

    return 0;
}
