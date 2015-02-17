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

#include "lua_transformnodebuilder.h"
#include "luacontext.h"
#include "exceptions.h"

#include "lua_matrix.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaTransformationNodeBuilder::className[] = "TransformationNodeBuilder";
const Luna2<LuaTransformationNodeBuilder>::RegType LuaTransformationNodeBuilder::methods[] =
{
  { "LinkTo", &LuaTransformationNodeBuilder::Lua_LinkTo },
  { "ClearMatrixStack", &LuaTransformationNodeBuilder::Lua_ClearMatrixStack },
  { "AddMatrix", &LuaTransformationNodeBuilder::Lua_AddMatrix },
  { 0, 0 }
};

LuaTransformationNodeBuilder::LuaTransformationNodeBuilder( lua_State* p_L )
: m_transformation_node( "transformation_node" )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "TransformationNodeBuilder ctor : bad number of args" );
		lua_error( p_L );		
	}
    const char* scene_name = luaL_checkstring( p_L, 1 );
    m_transformation_node.SetSceneName( scene_name );
    
    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();
    m_transformation_node.SetContent( &m_transformation );
}

LuaTransformationNodeBuilder::~LuaTransformationNodeBuilder( void ) 
{
}

int LuaTransformationNodeBuilder::Lua_LinkTo( lua_State* p_L )
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
    m_transformation_node.GetSceneName( scene_name );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "TransformationNodeBuilder:LinkTo" );
        props.AddPropValue<dsstring>( "scenegraph_name", scenegraph_name );        
        props.AddPropValue<dsstring>( "parent_name", parent_name );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<BaseSceneNode*>( "node", &m_transformation_node );

        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaTransformationNodeBuilder::Lua_ClearMatrixStack( lua_State* p_L )
{
    m_transformation.ClearAll();
    return 0;
}

int LuaTransformationNodeBuilder::Lua_AddMatrix( lua_State* p_L )
{
    LuaMatrix* mat = Luna2<LuaMatrix>::check( p_L, 1 );

    m_transformation.PushMatrix( mat->m_mat );
    return 0;
}

