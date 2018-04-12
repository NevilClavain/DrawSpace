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
#include "luaclass_fpstransformaspectimpl.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;

const char LuaClass_FPSTransformAspectImpl::className[] = "FPSTransformAspectImpl";
const Luna<LuaClass_FPSTransformAspectImpl>::RegType LuaClass_FPSTransformAspectImpl::methods[] =
{
    { "configure", &LuaClass_FPSTransformAspectImpl::LUA_configure },
    { "update", &LuaClass_FPSTransformAspectImpl::LUA_update },
    { "read", &LuaClass_FPSTransformAspectImpl::LUA_read },
    { "release", &LuaClass_FPSTransformAspectImpl::LUA_release },
	{ 0, 0 }
};

LuaClass_FPSTransformAspectImpl::LuaClass_FPSTransformAspectImpl( lua_State* p_L ) :
m_entity_transform_aspect( NULL )
{
}

LuaClass_FPSTransformAspectImpl::~LuaClass_FPSTransformAspectImpl( void )
{
}

int LuaClass_FPSTransformAspectImpl::LUA_configure( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 7 )
	{
        LUA_ERROR( "FPSTransformAspectImpl::configure : argument(s) missing" );
	}

    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 1 );
    if( NULL == lua_ent )
    {
        LUA_ERROR( "FPSTransformAspectImpl::configure : argument 1 must be of type LuaClass_Entity" );
    }

    dsreal yaw = luaL_checknumber( p_L, 2 );
    dsreal pitch = luaL_checknumber( p_L, 3 );

    dsreal x = luaL_checknumber( p_L, 4 );
    dsreal y = luaL_checknumber( p_L, 5 );
    dsreal z = luaL_checknumber( p_L, 6 );

    bool ymvt = luaL_checkint( p_L, 7 );

    DrawSpace::Core::Entity& entity = lua_ent->GetEntity();

    // recupere l'aspect transfo s'il existe pour cette entitee
    TransformAspect* transform_aspect = entity.GetAspect<TransformAspect>();
    if( transform_aspect )
    {
        transform_aspect->SetImplementation( &m_fps_transformer );
        m_entity_transform_aspect = transform_aspect;

        LUA_TRY
        {
            transform_aspect->SetImplementation( &m_fps_transformer );
            transform_aspect->AddComponent<dsreal>( "yaw", yaw );
            transform_aspect->AddComponent<dsreal>( "pitch", pitch );

            transform_aspect->AddComponent<Vector>( "speed" );
            transform_aspect->AddComponent<Matrix>( "pos" );

            transform_aspect->GetComponent<Matrix>( "pos" )->getPurpose().Translation( Vector( x, y, z, 1.0 ) );

            transform_aspect->AddComponent<bool>( "ymvt", ymvt );            

        } LUA_CATCH;        
    }
    else
    {
        m_entity_transform_aspect = NULL;
        LUA_ERROR( "FPSTransformAspectImpl::configure : entity passed on arg has no transform aspect" );    
    }

    return 0;
}

int LuaClass_FPSTransformAspectImpl::LUA_release( lua_State* p_L )
{
    if( m_entity_transform_aspect )
    {
        LUA_TRY
        {
            m_entity_transform_aspect->RemoveComponent<dsreal>( "yaw" );
            m_entity_transform_aspect->RemoveComponent<dsreal>( "pitch" );

            m_entity_transform_aspect->RemoveComponent<Vector>( "speed" );
            m_entity_transform_aspect->RemoveComponent<Matrix>( "pos" );

            m_entity_transform_aspect->RemoveComponent<bool>( "ymvt" );  
            
            m_entity_transform_aspect->RemoveImplementation();

        } LUA_CATCH;
    
        m_entity_transform_aspect = NULL;
    }
    else
    {
        LUA_ERROR( "FPSTransformAspectImpl::release : no transform aspect" );
    }
    return 0;
}

int LuaClass_FPSTransformAspectImpl::LUA_update( lua_State* p_L )
{
    
    if( !m_entity_transform_aspect )
    {
        LUA_ERROR( "FPSTransformAspectImpl::update : no transform aspect" );
    }

	int argc = lua_gettop( p_L );
	if( argc < 9 )
	{
        LUA_ERROR( "FPSTransformAspectImpl::update : argument(s) missing" );
	}
    
    LUA_TRY
    {
        dsreal yaw = luaL_checknumber( p_L, 1 );
        dsreal pitch = luaL_checknumber( p_L, 2 );

        dsreal x = luaL_checknumber( p_L, 3 );
        dsreal y = luaL_checknumber( p_L, 4 );
        dsreal z = luaL_checknumber( p_L, 5 );

        bool ymvt = luaL_checkint( p_L, 6 );
        
        dsreal xspeed = luaL_checknumber( p_L, 7 );
        dsreal yspeed = luaL_checknumber( p_L, 8 );
        dsreal zspeed = luaL_checknumber( p_L, 9 );


        m_entity_transform_aspect->GetComponent<dsreal>( "yaw" )->getPurpose() = yaw;
        m_entity_transform_aspect->GetComponent<dsreal>( "pitch" )->getPurpose() = pitch;

        m_entity_transform_aspect->GetComponent<Matrix>( "pos" )->getPurpose().Translation( Vector( x, y, z, 1.0 ) );

        m_entity_transform_aspect->GetComponent<bool>( "ymvt" )->getPurpose() = ymvt;

        Vector speed( xspeed, yspeed, zspeed, 1.0 );
        m_entity_transform_aspect->GetComponent<Vector>( "speed" )->getPurpose() = speed;
        
    } LUA_CATCH;
    
    return 0;
}

int LuaClass_FPSTransformAspectImpl::LUA_read( lua_State* p_L )
{
    if( !m_entity_transform_aspect )
    {
        LUA_ERROR( "FPSTransformAspectImpl::read : no transform aspect" );
    }

    LUA_TRY
    {

        dsreal yaw = m_entity_transform_aspect->GetComponent<dsreal>( "yaw" )->getPurpose();
        dsreal pitch = m_entity_transform_aspect->GetComponent<dsreal>( "pitch" )->getPurpose();

        Matrix mat = m_entity_transform_aspect->GetComponent<Matrix>( "pos" )->getPurpose();

        dsreal x = mat( 3, 0 );
        dsreal y = mat( 3, 1 );
        dsreal z = mat( 3, 2 );

        bool ymvt = m_entity_transform_aspect->GetComponent<bool>( "ymvt" )->getPurpose();

        Vector speed = m_entity_transform_aspect->GetComponent<Vector>( "speed" )->getPurpose();
    
        lua_pushnumber( p_L, yaw );
        lua_pushnumber( p_L, pitch );
        lua_pushnumber( p_L, x );
        lua_pushnumber( p_L, y );
        lua_pushnumber( p_L, z );
        lua_pushinteger( p_L, ymvt );

        lua_pushinteger( p_L, speed[0] );
        lua_pushinteger( p_L, speed[1] );
        lua_pushinteger( p_L, speed[2] );

    } LUA_CATCH;

    return 9;
}