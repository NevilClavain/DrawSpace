/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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
  { "LoadScript", &LuaCameraPointNode::Lua_LoadScript },
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

int LuaCameraPointNode::Lua_LoadScript( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "LoadScript : bad number of args" );
		lua_error( p_L );		
	}
    const char* filepath = luaL_checkstring( p_L, 1 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "CameraPointNode:LoadScript" );
        props.AddPropValue<dsstring>( "filepath", filepath );
        if( m_existing_camera_node )
        {
            props.AddPropValue<BaseSceneNode*>( "node", m_existing_camera_node );
        }
        else
        {
            props.AddPropValue<BaseSceneNode*>( "node", &m_camera_node );
        }

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}