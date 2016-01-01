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

#include "lua_camerapointnode.h"
#include "luacontext.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;

const char LuaCameraPointNode::className[] = "CameraPointNode";
const Luna2<LuaCameraPointNode>::RegType LuaCameraPointNode::methods[] =
{
  { "LinkTo", &LuaCameraPointNode::Lua_LinkTo },
  { "LockOn", &LuaCameraPointNode::Lua_LockOn },
  { "Unlock", &LuaCameraPointNode::Lua_Unlock },
  { "UpdateProjectionZNear", &LuaCameraPointNode::Lua_UpdateProjectionZNear },
  { 0, 0 }
};

LuaCameraPointNode::LuaCameraPointNode( lua_State* p_L )
: m_camera_node( "camera_node" ),
m_existing_camera_node( NULL )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
		lua_pushstring( p_L, "CameraPointNode ctor : bad number of args" );
		lua_error( p_L );		
	}
    const char* scene_name = luaL_checkstring( p_L, 1 );
    m_camera_node.SetSceneName( scene_name );

    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "CameraPointNode:CameraPointNode" );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<SceneNode<CameraPoint>**>( "existing_node", &m_existing_camera_node );

        (*m_scriptcalls_handler)( props );
    }
}

LuaCameraPointNode::~LuaCameraPointNode( void ) 
{
}

int LuaCameraPointNode::Lua_LinkTo( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "LinkTo : bad number of args" );
		lua_error( p_L );		
	}
    const char* scenegraph_name = luaL_checkstring( p_L, 1 );
    const char* parent_name = luaL_checkstring( p_L, 2 );

    dsstring scene_name;
    m_camera_node.GetSceneName( scene_name );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "CameraPointNode:LinkTo" );
        props.AddPropValue<dsstring>( "scenegraph_name", scenegraph_name );        
        props.AddPropValue<dsstring>( "parent_name", parent_name );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<BaseSceneNode*>( "node", &m_camera_node );        

        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaCameraPointNode::Lua_LockOn( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "LockOn : bad number of args" );
		lua_error( p_L );		
	}
    const char* nodename_input = luaL_checkstring( p_L, 1 );

    SceneNodeGraph* sc_owner;

    if( m_existing_camera_node )
    {
        sc_owner = m_existing_camera_node->GetSceneNodeGraph();
    }
    else
    {
        sc_owner = m_camera_node.GetSceneNodeGraph();
    }

    std::vector<BaseSceneNode*> n_list = sc_owner->GetAllNodesList();
    for( size_t i = 0; i < n_list.size(); i++ )
    {
        dsstring current_node_name;
        n_list[i]->GetSceneName( current_node_name );

        if( current_node_name == nodename_input )
        {
            if( m_existing_camera_node )
            {
                m_existing_camera_node->GetContent()->Lock( n_list[i] );
            }
            else
            {
                m_camera_node.GetContent()->Lock( n_list[i] );
            }
        }
    }          
    return 0;
}

int LuaCameraPointNode::Lua_Unlock( lua_State* p_L )
{
    if( m_existing_camera_node )
    {
        m_existing_camera_node->GetContent()->Unlock();
    }
    else
    {
        m_camera_node.GetContent()->Unlock();
    }
    return 0;
}

int LuaCameraPointNode::Lua_UpdateProjectionZNear( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "UpdateProjectionZNear : bad number of args" );
		lua_error( p_L );		
	}
    dsreal znear = luaL_checknumber( p_L, 1 );

    if( m_existing_camera_node )
    {
        m_existing_camera_node->GetContent()->UpdateProjectionZNear( znear );
    }
    else
    {
        m_camera_node.GetContent()->UpdateProjectionZNear( znear );
    }

    return 0;
}
