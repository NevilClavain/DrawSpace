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

#include "lua_inertbodynode.h"
#include "luacontext.h"
#include "lua_vector.h"
#include "exceptions.h"
#include "lua_matrix.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

const char LuaInertBodyNode::className[] = "InertBodyNode";
const Luna2<LuaInertBodyNode>::RegType LuaInertBodyNode::methods[] =
{ 
    { "IsValid", &LuaInertBodyNode::Lua_IsValid },
    { "AddInitialAttitudeMatrix", &LuaInertBodyNode::Lua_AddInitialAttitudeMatrix },
    { "SetShapeDescrSphere", &LuaInertBodyNode::Lua_SetShapeDescrSphere },
    { "SetShapeDescrBox", &LuaInertBodyNode::Lua_SetShapeDescrBox },
    { "SetMass", &LuaInertBodyNode::Lua_SetMass },
    { "SetInitialState", &LuaInertBodyNode::Lua_SetInitialState },

    { "GetShapeType", &LuaInertBodyNode::Lua_GetShapeType },
    { "GetShapeDescrBoxDimX", &LuaInertBodyNode::Lua_GetShapeDescrBoxDimX },
    { "GetShapeDescrBoxDimY", &LuaInertBodyNode::Lua_GetShapeDescrBoxDimY },
    { "GetShapeDescrBoxDimZ", &LuaInertBodyNode::Lua_GetShapeDescrBoxDimZ },
    { "GetShapeDescrSphereRadius", &LuaInertBodyNode::Lua_GetShapeDescrSphereRadius },

    { "UpdateState", &LuaInertBodyNode::Lua_UpdateState },

    { "LinkTo", &LuaInertBodyNode::Lua_LinkTo },
    { 0 }
};

LuaInertBodyNode::LuaInertBodyNode( lua_State* p_L ) :
m_inertbody_node( "inertbody_node" ),
m_existing_inertbody_node( NULL ),
m_initial_state( false )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "InertBodyNode ctor : bad number of args" );
		lua_error( p_L );		
	}
    const char* scene_name = luaL_checkstring( p_L, 1 );
    m_inertbody_node.SetSceneName( scene_name );

    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "InertBodyNode:InertBodyNode" );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<SceneNode<InertBody>**>( "existing_node", &m_existing_inertbody_node );

        (*m_scriptcalls_handler)( props );

        if( m_existing_inertbody_node )
        {
            m_existing_inertbody_node->GetContent()->GetParameters( m_params );
        }
    }
}

LuaInertBodyNode::~LuaInertBodyNode( void ) 
{
}

int LuaInertBodyNode::Lua_IsValid( lua_State* p_L )
{
    int status = 0;
    if( m_existing_inertbody_node || m_inertbody_node.GetContent() )
    {
        status = 1;
    }

    lua_pushinteger( p_L, status );
    return 1;
}

int LuaInertBodyNode::Lua_AddInitialAttitudeMatrix( lua_State* p_L )
{
    int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "AddInitialAttitudeMatrix : bad number of args" );
		lua_error( p_L );		
	}

    LuaMatrix* mat = Luna2<LuaMatrix>::check( p_L, 1 );
    if( !mat )
    {
		lua_pushstring( p_L, "AddInitialAttitudeMatrix : Matrix expected for arg 1" );
		lua_error( p_L );        
    }

    m_initial_att_transform.PushMatrix( mat->m_mat );
    return 0;
}

int LuaInertBodyNode::Lua_SetShapeDescrSphere( lua_State* p_L )
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

int LuaInertBodyNode::Lua_SetShapeDescrBox( lua_State* p_L )
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

int LuaInertBodyNode::Lua_SetMass( lua_State* p_L )
{
    int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetMass : bad number of args" );
		lua_error( p_L );		
	}
    m_params.mass = luaL_checknumber( p_L, 1 );
    return 0;
}


int LuaInertBodyNode::Lua_GetShapeType( lua_State* p_L )
{
    lua_pushinteger( p_L, m_params.shape_descr.shape );
    return 1;
}

int LuaInertBodyNode::Lua_GetShapeDescrBoxDimX( lua_State* p_L )
{
    lua_pushnumber( p_L, m_params.shape_descr.box_dims[0] );
    return 1;
}

int LuaInertBodyNode::Lua_GetShapeDescrBoxDimY( lua_State* p_L )
{
    lua_pushnumber( p_L, m_params.shape_descr.box_dims[1] );
    return 1;
}

int LuaInertBodyNode::Lua_GetShapeDescrBoxDimZ( lua_State* p_L )
{
    lua_pushnumber( p_L, m_params.shape_descr.box_dims[2] );
    return 1;
}

int LuaInertBodyNode::Lua_GetShapeDescrSphereRadius( lua_State* p_L )
{
    lua_pushnumber( p_L, m_params.shape_descr.sphere_radius );
    return 1;
}

int LuaInertBodyNode::Lua_SetInitialState( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetInitialState : bad number of args" );
		lua_error( p_L );		
	}

    m_initial_state = luaL_checkinteger( p_L, 1 );
    return 0;
}

int LuaInertBodyNode::Lua_LinkTo( lua_State* p_L )
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
    m_inertbody_node.GetSceneName( scene_name );

    if( m_scriptcalls_handler )
    {

        Matrix mat;
        m_initial_att_transform.BuildResult();
        m_initial_att_transform.GetResult( &mat );
        m_params.initial_attitude = mat;

        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "InertBodyNode:LinkTo" );
        props.AddPropValue<dsstring>( "scenegraph_name", scenegraph_name );        
        props.AddPropValue<dsstring>( "parent_name", parent_name );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<dsstring>( "world_name", world_name );
        props.AddPropValue<bool>( "init_state", m_initial_state );
        props.AddPropValue<BaseSceneNode*>( "node", &m_inertbody_node );
        props.AddPropValue<DrawSpace::Dynamics::Body::Parameters>( "descriptor", m_params );

        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaInertBodyNode::Lua_UpdateState( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "UpdateState : bad number of args" );
		lua_error( p_L );		
	}
    bool state = luaL_checkinteger( p_L, 1 );

    if( m_existing_inertbody_node )
    {
        m_existing_inertbody_node->GetContent()->Enable( state );
    }
    else
    {
        m_inertbody_node.GetContent()->Enable( state );
    }

    return 0;
}
