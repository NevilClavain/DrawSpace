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

#include "lua_chunknode.h"
#include "luacontext.h"
#include "lua_vector.h"
#include "lua_texture.h"
#include "luautils.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaChunkNode::className[] = "ChunkNode";
const Luna2<LuaChunkNode>::RegType LuaChunkNode::methods[] =
{  
  { "SetMesheName", &LuaChunkNode::Lua_SetMesheName },
  { "SetImpostorsDisplayList", &LuaChunkNode::Lua_SetImpostorsDisplayList },
  { "RegisterPassSlot", &LuaChunkNode::Lua_RegisterPassSlot },
  { "SetPassSlotFxName", &LuaChunkNode::Lua_SetPassSlotFxName },
  { "SetPassSlotRenderingOrder", &LuaChunkNode::Lua_SetPassSlotRenderingOrder },
  { "SetPassSlotTextureName", &LuaChunkNode::Lua_SetPassSlotTextureName },
  { "AddPassSlotShaderParam", &LuaChunkNode::Lua_AddPassSlotShaderParam },
  { "UpdateShaderParam", &LuaChunkNode::Lua_UpdateShaderParam },
  { "LinkTo", &LuaChunkNode::Lua_LinkTo },
  { "UpdatePassSlotTexture", &LuaChunkNode::Lua_UpdatePassSlotTexture },
  { 0 }
};

LuaChunkNode::LuaChunkNode( lua_State* p_L ) :
m_chunk_node( "chunk_node" )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "ChunkNode ctor : bad number of args" );
		lua_error( p_L );		
	}
    const char* scene_name = luaL_checkstring( p_L, 1 );
    m_chunk_node.SetSceneName( scene_name );
    m_descriptor.scene_name = scene_name;

    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();    
}

LuaChunkNode::~LuaChunkNode( void ) 
{
}

int LuaChunkNode::Lua_LinkTo( lua_State* p_L )
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
    m_chunk_node.GetSceneName( scene_name );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "ChunkNode:LinkTo" );
        props.AddPropValue<dsstring>( "scenegraph_name", scenegraph_name );        
        props.AddPropValue<dsstring>( "parent_name", parent_name );
        props.AddPropValue<dsstring>( "scene_name", scene_name );
        props.AddPropValue<BaseSceneNode*>( "node", &m_chunk_node );
        props.AddPropValue<DrawSpace::Utils::ChunkDescriptor>( "descriptor", m_descriptor );

        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaChunkNode::Lua_SetMesheName( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetMesheName : bad number of args" );
		lua_error( p_L );		
	}
    const char* meshe_name = luaL_checkstring( p_L, 1 );
    m_descriptor.meshe = meshe_name;

    return 0;
}

int LuaChunkNode::Lua_SetImpostorsDisplayList( lua_State* p_L )
{
    m_descriptor.impostors.clear();

    DrawSpace::ImpostorsDisplayListEntry idle;

	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetImpostorsDisplayList : bad number of args" );
		lua_error( p_L );		
	}

    if( !lua_istable( p_L, 1 ) )
    {
        lua_pushstring( p_L, "SetImpostorsDisplayList : table expected" );
        lua_error( p_L );
    }

    // http://www.fxcodebase.com/documents/IndicoreSDK.fr/lua/lua_next.html
    // http://www.lua.org/pil/25.1.html
   
    lua_pushnil( p_L );  /* 1�re cl� */
    while( lua_next( p_L, 1 ) != 0 ) 
    {          
       /* utilise la 'cl�' (� l'index -2) et la 'valeur' (� l'index -1) */
        
        if( !lua_istable( p_L, -1 ) )
        {
            lua_pushstring( p_L, "SetImpostorsDisplayList : unexpected table structure" );
            lua_error( p_L );
        }

        // aller chercher l'entree "pos" de la sous table 
        if( !push_namedluatable( p_L, "pos" ) )
        {
            lua_pushstring( p_L, "SetImpostorsDisplayList : unexpected table structure" );
            lua_error( p_L ); 
        }
        
        idle.localpos[0] = get_namedrealfromtable( p_L, "x" );
        idle.localpos[1] = get_namedrealfromtable( p_L, "y" );
        idle.localpos[2] = get_namedrealfromtable( p_L, "z" );


        lua_pop( p_L, 1 ); // pop sous table pos



        if( !push_namedluatable( p_L, "scale" ) )
        {
            lua_pushstring( p_L, "SetImpostorsDisplayList : unexpected table structure" );
            lua_error( p_L ); 
        }


        idle.width_scale = get_namedrealfromtable( p_L, "width" );
        idle.height_scale = get_namedrealfromtable( p_L, "height" );

        lua_pop( p_L, 1 ); // pop sous table scale



        if( !push_namedluatable( p_L, "uv" ) )
        {
            lua_pushstring( p_L, "SetImpostorsDisplayList : unexpected table structure" );
            lua_error( p_L ); 
        }


        idle.u1 = get_namedrealfromtable( p_L, "u1" );
        idle.v1 = get_namedrealfromtable( p_L, "v1" );

        idle.u2 = get_namedrealfromtable( p_L, "u2" );
        idle.v2 = get_namedrealfromtable( p_L, "v2" );

        idle.u3 = get_namedrealfromtable( p_L, "u3" );
        idle.v3 = get_namedrealfromtable( p_L, "v3" );

        idle.u4 = get_namedrealfromtable( p_L, "u4" );
        idle.v4 = get_namedrealfromtable( p_L, "v4" );

        lua_pop( p_L, 1 ); // pop sous table uv

        /* enl�ve la 'valeur' ; garde la 'cl�' pour la prochaine it�ration */
        lua_pop( p_L, 1 );

        m_descriptor.impostors.push_back( idle );
    }
    
    return 0;
}

int LuaChunkNode::Lua_RegisterPassSlot( lua_State* p_L )
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
    m_descriptor.passes_slots[pass_name] = pass_descriptor;

    return 0;
}

int LuaChunkNode::Lua_SetPassSlotFxName( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetPassSlotFxName : bad number of args" );
		lua_error( p_L );		
	}
    const char* pass_name = luaL_checkstring( p_L, 1 );
    const char* fx_name = luaL_checkstring( p_L, 2 );

    if( 0 == m_descriptor.passes_slots.count( pass_name ) )
    {
        _DSEXCEPTION( "scripting FATAL : unknown pass" );
    }
    else
    {
        m_descriptor.passes_slots[pass_name].fx_name = fx_name;
    }
    return 0;
}

int LuaChunkNode::Lua_SetPassSlotRenderingOrder( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetPassSlotRenderingOrder : bad number of args" );
		lua_error( p_L );		
	}
    const char* pass_name = luaL_checkstring( p_L, 1 );
    long order = luaL_checkinteger( p_L, 2 );
    
    if( 0 == m_descriptor.passes_slots.count( pass_name ) )
    {
        _DSEXCEPTION( "scripting FATAL : unknown pass" );
    }
    else
    {
        m_descriptor.passes_slots[pass_name].rendering_order = order;
    }
    return 0;
}

int LuaChunkNode::Lua_SetPassSlotTextureName( lua_State* p_L )
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
    
    if( 0 == m_descriptor.passes_slots.count( pass_name ) )
    {
        _DSEXCEPTION( "scripting FATAL : unknown pass" );
    }
    else
    {
        m_descriptor.passes_slots[pass_name].textures[stage] = texture_name;
    }
    return 0;
}

int LuaChunkNode::Lua_AddPassSlotShaderParam( lua_State* p_L )
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

    if( 0 == m_descriptor.passes_slots.count( pass_name ) )
    {
        _DSEXCEPTION( "scripting FATAL : unknown pass" );
    }
    else
    {
        m_descriptor.passes_slots[pass_name].shader_params.push_back( psp );
    }
    return 0;
}

int LuaChunkNode::Lua_UpdateShaderParam( lua_State* p_L )
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
        props.AddPropValue<dsstring>( "script_call_id", "ChunkNode:UpdateShaderParam" );

        props.AddPropValue<dsstring>( "scene_name", m_descriptor.scene_name );
        props.AddPropValue<dsstring>( "pass_name", pass_name );
        props.AddPropValue<dsstring>( "id", id );
        props.AddPropValue<DrawSpace::Utils::Vector>( "value", value->m_vector );

        (*m_scriptcalls_handler)( props );
    }
    
    return 0;
}

int LuaChunkNode::Lua_UpdatePassSlotTexture( lua_State* p_L )
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
    m_chunk_node.GetContent()->GetPassesNodesList( passesnodes );

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
