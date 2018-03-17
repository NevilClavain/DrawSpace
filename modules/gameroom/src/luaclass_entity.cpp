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

#include "luaclass_entity.h"
#include "luaclass_renderpassnodegraph.h"
#include "renderingaspect.h"
#include "timeaspect.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;

const char LuaClass_Entity::className[] = "Entity";
const Luna<LuaClass_Entity>::RegType LuaClass_Entity::methods[] =
{
    { "add_renderingaspect", &LuaClass_Entity::LUA_addrenderingaspect },
    { "add_timeaspect", &LuaClass_Entity::LUA_addtimeaspect },
    { "connect_renderingaspect_rendergraph", &LuaClass_Entity::LUA_connect_renderingaspect_rendergraph },
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

int LuaClass_Entity::LUA_addrenderingaspect( lua_State* p_L )
{
    m_entity.AddAspect<RenderingAspect>();
    return 0;
}


int LuaClass_Entity::LUA_addtimeaspect( lua_State* p_L )
{
    TimeAspect* time_aspect = m_entity.AddAspect<TimeAspect>();

    time_aspect->AddComponent<TimeManager>( "time_manager" );
    time_aspect->AddComponent<TimeAspect::TimeScale>( "time_scale", TimeAspect::NORMAL_TIME );
    time_aspect->AddComponent<dsstring>( "output_formated_datetime", "..." );
    time_aspect->AddComponent<dstime>( "time", 0 );
    time_aspect->AddComponent<int>( "output_fps" );
    time_aspect->AddComponent<int>( "output_world_nbsteps" );

    time_aspect->AddComponent<dsreal>( "output_time_factor" );
    return 0;
}

int LuaClass_Entity::LUA_connect_renderingaspect_rendergraph( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "Entity::connect_renderingaspect_rendergraph : argument(s) missing" );
		lua_error( p_L );		
	}

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check( p_L, 1 );

    RenderingAspect* rendering_aspect = m_entity.GetAspect<RenderingAspect>();
    if( rendering_aspect )
    {
        rendering_aspect->AddImplementation( &lua_rg->GetPassesRenderAspectImpl() );
    }
    else
    {
		lua_pushstring( p_L, "Entity::connect_renderingaspect_rendergraph : entity has no rendering aspect. Call add_renderingaspect() method first" );
		lua_error( p_L );    
    }

    return 0;
}