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

#include "lua_inertbodynode.h"
#include "luacontext.h"
#include "lua_vector.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;

const char LuaInertBodyNode::className[] = "InertBodyNode";
const Luna2<LuaInertBodyNode>::RegType LuaInertBodyNode::methods[] =
{ 
    { "IsValid", &LuaInertBodyNode::Lua_IsValid },
    { "LinkTo", &LuaInertBodyNode::Lua_LinkTo },
    { 0 }
};

LuaInertBodyNode::LuaInertBodyNode( lua_State* p_L ) :
m_inertbody_node( "inertbody_node" ),
m_existing_inertbody_node( NULL )
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

int LuaInertBodyNode::Lua_LinkTo( lua_State* p_L )
{

    return 0;
}
