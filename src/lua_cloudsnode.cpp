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

#include "lua_cloudsnode.h"
#include "luacontext.h"
#include "lua_vector.h"
#include "lua_texture.h"
#include "luautils.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaCloudsNode::className[] = "Clouds";
const Luna2<LuaCloudsNode>::RegType LuaCloudsNode::methods[] =
{  
  { "RegisterPassSlot", &LuaCloudsNode::Lua_RegisterPassSlot },
  { "SetPassSlotFxName", &LuaCloudsNode::Lua_SetPassSlotFxName },
  { "SetPassSlotRenderingOrder", &LuaCloudsNode::Lua_SetPassSlotRenderingOrder },
  { "SetPassSlotTextureName", &LuaCloudsNode::Lua_SetPassSlotTextureName },
  { "AddPassSlotShaderParam", &LuaCloudsNode::Lua_AddPassSlotShaderParam },
  { "UpdateShaderParam", &LuaCloudsNode::Lua_UpdateShaderParam },
  { "LinkTo", &LuaCloudsNode::Lua_LinkTo },
  { "UpdatePassSlotTexture", &LuaCloudsNode::Lua_UpdatePassSlotTexture },
  { 0 }
};

LuaCloudsNode::LuaCloudsNode( lua_State* p_L ) :
m_clouds_node( "clouds_node" )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "CloudsNode ctor : bad number of args" );
		lua_error( p_L );		
	}
    const char* scene_name = luaL_checkstring( p_L, 1 );
    m_clouds_node.SetSceneName( scene_name );
    m_descriptor.chunk_descriptor.scene_name = scene_name;

    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();    
}

LuaCloudsNode::~LuaCloudsNode( void ) 
{
}

int LuaCloudsNode::Lua_EnableDetails( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "EnableDetails : bad number of args" );
		lua_error( p_L );		
	}
    m_descriptor.details = luaL_checkinteger( p_L, 1 );
    return 0;
}

int LuaCloudsNode::Lua_SetSortingDistance( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetSortingDistance : bad number of args" );
		lua_error( p_L );		
	}
    m_descriptor.sorting_distance = luaL_checknumber( p_L, 1 );

    return 0;
}

int LuaCloudsNode::Lua_SetProceduralFilePath( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetProceduralFilePath : bad number of args" );
		lua_error( p_L );		
	}
    const char* path = luaL_checkstring( p_L, 1 );
    m_descriptor.rules_filepath = path;

    return 0;
}

int LuaCloudsNode::Lua_LinkTo( lua_State* p_L )
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
    m_clouds_node.GetSceneName( scene_name );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "Clouds:LinkTo" );
        props.AddPropValue<dsstring>( "scenegraph_name", scenegraph_name );        
        props.AddPropValue<dsstring>( "parent_name", parent_name );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<BaseSceneNode*>( "node", &m_clouds_node );
        props.AddPropValue<DrawSpace::Utils::CloudsDescriptor>( "descriptor", m_descriptor );

        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaCloudsNode::Lua_RegisterPassSlot( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "RegisterPassSlot : bad number of args" );
		lua_error( p_L );
	}
    const char* pass_name = luaL_checkstring( p_L, 1 );

    DrawSpace::Utils::ChunkPassDescriptor pass_descriptor;
    pass_descriptor.rendering_order = 10000;
    m_descriptor.chunk_descriptor.passes_slots[pass_name] = pass_descriptor;

    return 0;
}

int LuaCloudsNode::Lua_SetPassSlotFxName( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetPassSlotFxName : bad number of args" );
		lua_error( p_L );		
	}
    const char* pass_name = luaL_checkstring( p_L, 1 );
    const char* fx_name = luaL_checkstring( p_L, 2 );

    if( 0 == m_descriptor.chunk_descriptor.passes_slots.count( pass_name ) )
    {
        _DSEXCEPTION( "scripting FATAL : unknown pass" );
    }
    else
    {
        m_descriptor.chunk_descriptor.passes_slots[pass_name].fx_name = fx_name;
    }
    return 0;
}

int LuaCloudsNode::Lua_SetPassSlotRenderingOrder( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetPassSlotRenderingOrder : bad number of args" );
		lua_error( p_L );		
	}
    const char* pass_name = luaL_checkstring( p_L, 1 );
    long order = luaL_checkinteger( p_L, 2 );
    
    if( 0 == m_descriptor.chunk_descriptor.passes_slots.count( pass_name ) )
    {
        _DSEXCEPTION( "scripting FATAL : unknown pass" );
    }
    else
    {
        m_descriptor.chunk_descriptor.passes_slots[pass_name].rendering_order = order;
    }
    return 0;
}

int LuaCloudsNode::Lua_SetPassSlotTextureName( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "SetPassSlotTextureName : bad number of args" );
		lua_error( p_L );		
	}
    const char* pass_name = luaL_checkstring( p_L, 1 );
    const char* texture_name = luaL_checkstring( p_L, 2 );
    long stage = luaL_checkinteger( p_L, 3 );
    
    if( 0 == m_descriptor.chunk_descriptor.passes_slots.count( pass_name ) )
    {
        _DSEXCEPTION( "scripting FATAL : unknown pass" );
    }
    else
    {
        m_descriptor.chunk_descriptor.passes_slots[pass_name].textures[stage] = texture_name;
    }
    return 0;
}

int LuaCloudsNode::Lua_AddPassSlotShaderParam( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 5 )
	{
		lua_pushstring( p_L, "AddPassSlotShaderParam : bad number of args" );
		lua_error( p_L );		
	}

    const char* pass_name = luaL_checkstring( p_L, 1 );
    const char* id = luaL_checkstring( p_L, 2 );
    long shader_index = luaL_checkinteger( p_L, 3 );
    long shader_register = luaL_checkinteger( p_L, 4 );
    LuaVector* value = Luna2<LuaVector>::check( p_L, 5 );

    DrawSpace::Utils::PassShaderParam psp;

    psp.id = id;
    psp.shader_index = shader_index;
    psp.shader_register = shader_register;
    psp.value = value->m_vector;

    if( 0 == m_descriptor.chunk_descriptor.passes_slots.count( pass_name ) )
    {
        _DSEXCEPTION( "scripting FATAL : unknown pass" );
    }
    else
    {
        m_descriptor.chunk_descriptor.passes_slots[pass_name].shader_params.push_back( psp );
    }
    return 0;
}

int LuaCloudsNode::Lua_UpdateShaderParam( lua_State* p_L )
{
    int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "UpdateShaderParam : bad number of args" );
		lua_error( p_L );		
	}

    const char* pass_name = luaL_checkstring( p_L, 1 );
    const char* id = luaL_checkstring( p_L, 2 );
    LuaVector* value = Luna2<LuaVector>::check( p_L, 3 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "Clouds:UpdateShaderParam" );

        props.AddPropValue<dsstring>( "scene_name", m_descriptor.chunk_descriptor.scene_name );
        props.AddPropValue<dsstring>( "pass_name", pass_name );
        props.AddPropValue<dsstring>( "id", id );
        props.AddPropValue<DrawSpace::Utils::Vector>( "value", value->m_vector );

        (*m_scriptcalls_handler)( props );
    }
    
    return 0;
}

int LuaCloudsNode::Lua_UpdatePassSlotTexture( lua_State* p_L )
{
    int argc = lua_gettop( p_L );
	if( argc != 3 )
	{
		lua_pushstring( p_L, "UpdatePassSlotTexture : bad number of args" );
		lua_error( p_L );		
	}

    const char* pass_name = luaL_checkstring( p_L, 1 );
    LuaTexture* lua_texture = Luna2<LuaTexture>::check( p_L, 2 );
    long stage = luaL_checkinteger( p_L, 3 );

    std::map<Pass*, DrawSpace::Core::RenderingNode*> passesnodes;
    m_clouds_node.GetContent()->GetPassesNodesList( passesnodes );

    RenderingNode* rn = NULL;
    IntermediatePass* ipass;

    for( std::map<Pass*, DrawSpace::Core::RenderingNode*>::iterator it = passesnodes.begin(); it != passesnodes.end(); ++it )
    {
        dsstring current_pass_name;
        it->first->GetSpecificName( current_pass_name );

        if( pass_name == current_pass_name )
        {
            ipass = static_cast<IntermediatePass*>( it->first );
            rn = it->second;
            break;
        }
    }

    if( !rn )
    {
		lua_pushstring( p_L, "UpdatePassSlotTexture : unknown pass" );
		lua_error( p_L );        
    }
    rn->SetTexture( lua_texture->m_texture, stage );
    ipass->GetRenderingQueue()->UpdateOutputQueue();
    
    return 0;
}