/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#include "lua_collidernode.h"
#include "luacontext.h"
#include "lua_vector.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

const char LuaColliderNode::className[] = "ColliderNode";
const Luna2<LuaColliderNode>::RegType LuaColliderNode::methods[] =
{ 
    { "IsValid", &LuaColliderNode::Lua_IsValid },
    { "SetShapeDescrSphere", &LuaColliderNode::Lua_SetShapeDescrSphere },
    { "SetShapeDescrBox", &LuaColliderNode::Lua_SetShapeDescrBox },

    { "GetShapeType", &LuaColliderNode::Lua_GetShapeType },
    { "GetShapeDescrBoxDimX", &LuaColliderNode::Lua_GetShapeDescrBoxDimX },
    { "GetShapeDescrBoxDimY", &LuaColliderNode::Lua_GetShapeDescrBoxDimY },
    { "GetShapeDescrBoxDimZ", &LuaColliderNode::Lua_GetShapeDescrBoxDimZ },
    { "GetShapeDescrSphereRadius", &LuaColliderNode::Lua_GetShapeDescrSphereRadius },

    { "LinkTo", &LuaColliderNode::Lua_LinkTo },
    { 0 }
};

LuaColliderNode::LuaColliderNode( lua_State* p_L ) :
m_collider_node( "collider_node" ),
m_existing_collider_node( NULL )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "ColliderNode ctor : bad number of args" );
		lua_error( p_L );		
	}
    const char* scene_name = luaL_checkstring( p_L, 1 );
    m_collider_node.SetSceneName( scene_name );

    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "ColliderNode:ColliderNode" );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<SceneNode<Collider>**>( "existing_node", &m_existing_collider_node );

        (*m_scriptcalls_handler)( props );

        if( m_existing_collider_node )
        {
            m_existing_collider_node->GetContent()->GetParameters( m_params );
        }
    }

}

LuaColliderNode::~LuaColliderNode( void )
{
}

int LuaColliderNode::Lua_IsValid( lua_State* p_L )
{
    int status = 0;
    if( m_existing_collider_node || m_collider_node.GetContent() )
    {
        status = 1;
    }

    lua_pushinteger( p_L, status );
    return 1;

}

int LuaColliderNode::Lua_SetShapeDescrSphere( lua_State* p_L )
{
    int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetShapeDescrSphere : bad number of args" );
		lua_error( p_L );		
	}
    m_params.shape_descr.sphere_radius = luaL_checknumber( p_L, 1 );    
    m_params.shape_descr.shape = Body::SPHERE_SHAPE;
    return 0;
}

int LuaColliderNode::Lua_SetShapeDescrBox( lua_State* p_L )
{
    int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetShapeDescrBox : bad number of args" );
		lua_error( p_L );		
	}
    LuaVector* vec = Luna2<LuaVector>::check( p_L, 1 );
    if( !vec )
    {
		lua_pushstring( p_L, "SetShapeDescrBox : Vector expected for arg 1" );
		lua_error( p_L );        
    }

    m_params.shape_descr.box_dims[0] = vec->m_vector[0];
    m_params.shape_descr.box_dims[1] = vec->m_vector[1];
    m_params.shape_descr.box_dims[2] = vec->m_vector[2];
    m_params.shape_descr.box_dims[3] = 1.0;

    m_params.shape_descr.shape = Body::BOX_SHAPE;

    return 0;
}

int LuaColliderNode::Lua_GetShapeType( lua_State* p_L )
{
    lua_pushinteger( p_L, m_params.shape_descr.shape );
    return 1;
}

int LuaColliderNode::Lua_GetShapeDescrBoxDimX( lua_State* p_L )
{
    lua_pushnumber( p_L, m_params.shape_descr.box_dims[0] );
    return 1;
}

int LuaColliderNode::Lua_GetShapeDescrBoxDimY( lua_State* p_L )
{
    lua_pushnumber( p_L, m_params.shape_descr.box_dims[1] );
    return 1;
}

int LuaColliderNode::Lua_GetShapeDescrBoxDimZ( lua_State* p_L )
{
    lua_pushnumber( p_L, m_params.shape_descr.box_dims[2] );
    return 1;
}

int LuaColliderNode::Lua_GetShapeDescrSphereRadius( lua_State* p_L )
{
    lua_pushnumber( p_L, m_params.shape_descr.sphere_radius );
    return 1;
}

int LuaColliderNode::Lua_LinkTo( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "LinkTo : bad number of args" );
		lua_error( p_L );		
	}
    const char* scenegraph_name = luaL_checkstring( p_L, 1 );
    const char* parent_name = luaL_checkstring( p_L, 2 );
    const char* world_name = luaL_checkstring( p_L, 3 );

    dsstring scene_name;
    m_collider_node.GetSceneName( scene_name );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "ColliderNode:LinkTo" );
        props.AddPropValue<dsstring>( "scenegraph_name", scenegraph_name );        
        props.AddPropValue<dsstring>( "parent_name", parent_name );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<dsstring>( "world_name", world_name );
        props.AddPropValue<BaseSceneNode*>( "node", &m_collider_node );
        props.AddPropValue<DrawSpace::Dynamics::Body::Parameters>( "descriptor", m_params );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

