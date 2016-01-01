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

#include "lua_drawspace.h"
#include "lua_real.h"
#include "luacontext.h"
#include "lua_camerapointnode.h"
#include "lua_vector.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

const char LuaDrawSpace::className[] = "DrawSpace";
const Luna2<LuaDrawSpace>::RegType LuaDrawSpace::methods[] =
{
  { "DisplayFramerate", &LuaDrawSpace::Lua_DisplayFramerate },
  { "DisplayCurrentCamera", &LuaDrawSpace::Lua_DisplayCurrentCamera },
  { "CreateSceneNodeGraph", &LuaDrawSpace::Lua_CreateSceneNodeGraph },
  { "CreateWorld", &LuaDrawSpace::Lua_CreateWorld },
  { "SetSceneNodeGraphCurrentCamera", &LuaDrawSpace::Lua_SetSceneNodeGraphCurrentCamera },
  { "SetWorldGravity", &LuaDrawSpace::Lua_SetWorldGravity },
  { "LoadKeyUpScript", &LuaDrawSpace::Lua_LoadKeyUpScript },
  { "LoadKeyDownScript", &LuaDrawSpace::Lua_LoadKeyDownScript },
  { "LoadMouseScript", &LuaDrawSpace::Lua_LoadMouseScript },
  { "AngleSpeedInc", &LuaDrawSpace::Lua_AngleSpeedInc },
  { "AngleSpeedDec", &LuaDrawSpace::Lua_AngleSpeedDec },
  { "TranslationSpeedInc", &LuaDrawSpace::Lua_TranslationSpeedInc },
  { "TranslationSpeedDec", &LuaDrawSpace::Lua_TranslationSpeedDec },
  { "GetSceneCameraName", &LuaDrawSpace::Lua_GetSceneCameraName },
  { "IsCurrentCamera", &LuaDrawSpace::Lua_IsCurrentCamera },
  { "SetScenegraphNodeEventCallback", &LuaDrawSpace::Lua_SetScenegraphNodeEventCallback },
  { "SetScenegraphEventCallback", &LuaDrawSpace::Lua_SetScenegraphEventCallback },
  { "UpdatePassShaderParam", &LuaDrawSpace::Lua_UpdatePassShaderParam },
  { 0 }
};


LuaDrawSpace::LuaDrawSpace( lua_State* p_L ) :
m_timer( NULL ),
m_L( p_L ),
m_ref( -1 ),
m_ref2( -1 )
{   
    m_scriptcalls_handler = LuaContext::GetInstance()->GetScriptCallsHandler();

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:DrawSpace" );        
        props.AddPropValue<DrawSpace::Utils::TimeManager**>( "timer_ref", &m_timer );

        (*m_scriptcalls_handler)( props );
    }

}

LuaDrawSpace::~LuaDrawSpace( void ) 
{
}

int LuaDrawSpace::Lua_DisplayFramerate( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != /*2*/ 1 )
	{
		lua_pushstring( p_L, "DisplayFramerate : bad number of args" );
		lua_error( p_L );		
	}

    bool display = ( (int)luaL_checkinteger( p_L, /*2*/ 1 ) != 0 ? true : false );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:DisplayFramerate" );
        props.AddPropValue<bool>( "state", display );

        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaDrawSpace::Lua_DisplayCurrentCamera( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "DisplayCurrentCamera : bad number of args" );
		lua_error( p_L );		
	}

    const char* scenegraphname = luaL_checkstring( p_L, 1 );
    bool display = ( (int)luaL_checkinteger( p_L, 2 ) != 0 ? true : false );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:DisplayCurrentCamera" );
        props.AddPropValue<dsstring>( "scenegraphname", scenegraphname );
        props.AddPropValue<bool>( "state", display );

        (*m_scriptcalls_handler)( props );
    }

    return 0;
}

int LuaDrawSpace::Lua_CreateSceneNodeGraph( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "CreateSceneNodeGraph : bad number of args" );
		lua_error( p_L );		
	}
    const char* name = luaL_checkstring( p_L, 1 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        ScenegraphDescr new_sc_descr;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:CreateSceneNodeGraph" );
        props.AddPropValue<dsstring>( "name", name );
        props.AddPropValue<SceneNodeGraph**>( "newsc_ptr", &new_sc_descr.scenenodegraph );
        props.AddPropValue<dsstring*>( "newsc_alias_ptr", &new_sc_descr.alias );

        (*m_scriptcalls_handler)( props );

        new_sc_descr.nodes_event_cb = new NodesEventCallback( this, &LuaDrawSpace::on_scenenode_evt );
        new_sc_descr.sc_event_cb = new ScenegraphEventCallback( this, &LuaDrawSpace::on_scenenodegraph_evt );

        new_sc_descr.scenenodegraph->RegisterNodesEvtHandler( new_sc_descr.nodes_event_cb );
        new_sc_descr.scenenodegraph->RegisterScenegraphEvtHandler( new_sc_descr.sc_event_cb );

        m_nodesevent_callbacks.push_back( new_sc_descr );
    }
    return 0;
}

int LuaDrawSpace::Lua_CreateWorld( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "CreateWorld : bad number of args" );
		lua_error( p_L );		
	}
    const char* name = luaL_checkstring( p_L, 1 );
    LuaVector* vec = Luna2<LuaVector>::check( p_L, 2 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        ScenegraphDescr new_sc_descr;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:CreateWorld" );
        props.AddPropValue<dsstring>( "name", name );
        props.AddPropValue<Utils::Vector>( "gravity", vec->m_vector );
        
        (*m_scriptcalls_handler)( props );
    }


    return 0;
}

int LuaDrawSpace::Lua_SetSceneNodeGraphCurrentCamera( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetSceneNodeGraphCurrentCamera : bad number of args" );
		lua_error( p_L );		
	}
    const char* scenegraphname = luaL_checkstring( p_L, 1 );
    const char* cameraname = luaL_checkstring( p_L, 2 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:SetSceneNodeGraphCurrentCamera" );
        props.AddPropValue<dsstring>( "scenegraphname", scenegraphname );
        props.AddPropValue<dsstring>( "cameraname", cameraname );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_SetWorldGravity( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "SetWorldGravity : bad number of args" );
		lua_error( p_L );		
	}
    const char* name = luaL_checkstring( p_L, 1 );
    LuaVector* vec = Luna2<LuaVector>::check( p_L, 2 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:SetWorldGravity" );
        props.AddPropValue<dsstring>( "worldname", name );
        props.AddPropValue<Utils::Vector>( "gravity", vec->m_vector );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_LoadKeyUpScript( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "LoadKeyUpScript : bad number of args" );
		lua_error( p_L );		
	}
    const char* filepath = luaL_checkstring( p_L, 1 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:LoadKeyUpScript" );
        props.AddPropValue<dsstring>( "filepath", filepath );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_LoadKeyDownScript( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "LoadKeyDownScript : bad number of args" );
		lua_error( p_L );		
	}
    const char* filepath = luaL_checkstring( p_L, 1 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:LoadKeyDownScript" );
        props.AddPropValue<dsstring>( "filepath", filepath );

        (*m_scriptcalls_handler)( props );
    }
    return 0;

}

int LuaDrawSpace::Lua_LoadMouseScript( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "LoadMouseScript : bad number of args" );
		lua_error( p_L );		
	}
    const char* filepath = luaL_checkstring( p_L, 1 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;

        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:LoadMouseScript" );
        props.AddPropValue<dsstring>( "filepath", filepath );

        (*m_scriptcalls_handler)( props );
    }
    return 0;
}

int LuaDrawSpace::Lua_AngleSpeedInc( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "AngleSpeedInc : bad number of args" );
		lua_error( p_L );		
	}
    LuaReal* real = Luna2<LuaReal>::check( p_L, 1 );
    if( !real )
    {
		lua_pushstring( p_L, "AngleSpeedInc : Real expected for arg 1" );
		lua_error( p_L );        
    }
    dsreal speed = luaL_checknumber( p_L, 2 );

    PropertyPool props;
    props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:AngleSpeedInc" );
    props.AddPropValue<dsreal>( "speed", speed );
    props.AddPropValue<dsreal*>( "realvar", &real->m_value );
    (*m_scriptcalls_handler)( props );

    return 0;
}

int LuaDrawSpace::Lua_AngleSpeedDec( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "AngleSpeedDec : bad number of args" );
		lua_error( p_L );		
	}
    LuaReal* real = Luna2<LuaReal>::check( p_L, 1 );
    if( !real )
    {
		lua_pushstring( p_L, "AngleSpeedDec : Real expected for arg 1" );
		lua_error( p_L );        
    }
    dsreal speed = luaL_checknumber( p_L, 2 );

    PropertyPool props;
    props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:AngleSpeedDec" );
    props.AddPropValue<dsreal>( "speed", speed );
    props.AddPropValue<dsreal*>( "realvar", &real->m_value );
    (*m_scriptcalls_handler)( props );

    return 0;
}

int LuaDrawSpace::Lua_TranslationSpeedInc( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "TranslationSpeedInc : bad number of args" );
		lua_error( p_L );		
	}
    LuaReal* real = Luna2<LuaReal>::check( p_L, 1 );
    if( !real )
    {
		lua_pushstring( p_L, "TranslationSpeedInc : Real expected for arg 1" );
		lua_error( p_L );        
    }
    dsreal speed = luaL_checknumber( p_L, 2 );

    PropertyPool props;
    props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:TranslationSpeedInc" );
    props.AddPropValue<dsreal>( "speed", speed );
    props.AddPropValue<dsreal*>( "realvar", &real->m_value );
    (*m_scriptcalls_handler)( props );

    return 0;
}

int LuaDrawSpace::Lua_TranslationSpeedDec( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "TranslationSpeedDec : bad number of args" );
		lua_error( p_L );		
	}
    LuaReal* real = Luna2<LuaReal>::check( p_L, 1 );
    if( !real )
    {
		lua_pushstring( p_L, "TranslationSpeedDec : Real expected for arg 1" );
		lua_error( p_L );        
    }
    dsreal speed = luaL_checknumber( p_L, 2 );

    PropertyPool props;
    props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:TranslationSpeedDec" );
    props.AddPropValue<dsreal>( "speed", speed );
    props.AddPropValue<dsreal*>( "realvar", &real->m_value );
    (*m_scriptcalls_handler)( props );

    return 0;
}

int LuaDrawSpace::Lua_GetSceneCameraName( lua_State* p_L )
{
    dsstring result;

	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "GetSceneCameraName : bad number of args" );
		lua_error( p_L );		
	}
    const char* scenegraphname = luaL_checkstring( p_L, 1 );
    
    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:GetSceneCameraName" );
        props.AddPropValue<dsstring>( "scenegraphname", scenegraphname );
        props.AddPropValue<dsstring*>( "result", &result );

        (*m_scriptcalls_handler)( props );
    }

    lua_pushstring( p_L, result.c_str() );
    return 1;
}

int LuaDrawSpace::Lua_IsCurrentCamera( lua_State* p_L )
{
    bool result = false;

	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "IsCurrentCamera : bad number of args" );
		lua_error( p_L );		
	}
    const char* scenegraphname = luaL_checkstring( p_L, 1 );

    LuaCameraPointNode* camnode = Luna2<LuaCameraPointNode>::check( p_L, 2 );
    if( !camnode )
    {
		lua_pushstring( p_L, "IsCurrentCamera : CameraPointNode expected for arg 2" );
		lua_error( p_L );        
    }

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:IsCurrentCamera" );
        props.AddPropValue<dsstring>( "scenegraphname", scenegraphname );

        if( camnode->m_existing_camera_node )
        {
            props.AddPropValue<BaseSceneNode*>( "camera_node", camnode->m_existing_camera_node );
        }
        else
        {
            props.AddPropValue<BaseSceneNode*>( "camera_node", &camnode->m_camera_node );
        }
        props.AddPropValue<bool*>( "result", &result );

        (*m_scriptcalls_handler)( props );
    }

    lua_pushinteger( p_L, result );
    return 1;
}

int LuaDrawSpace::Lua_SetScenegraphNodeEventCallback( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetScenegraphNodeEventCallback : bad number of args" );
		lua_error( p_L );		
	}
    
    int status = lua_isfunction( p_L, 1 );

    if( status > 0 )
    {
        if( m_ref != -1 )
        {
            luaL_unref( p_L, LUA_REGISTRYINDEX, m_ref );
        }
        m_ref = luaL_ref( p_L, LUA_REGISTRYINDEX );
    }
    else
    {
		lua_pushstring( p_L, "SetScenegraphNodeEventCallback : bad arg type, function expected" );
		lua_error( p_L );		
    }
    return 0;
}

int LuaDrawSpace::Lua_SetScenegraphEventCallback( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 1 )
	{
		lua_pushstring( p_L, "SetScenegraphEventCallback : bad number of args" );
		lua_error( p_L );		
	}
    
    int status = lua_isfunction( p_L, 1 );

    if( status > 0 )
    {
        if( m_ref2 != -1 )
        {
            luaL_unref( p_L, LUA_REGISTRYINDEX, m_ref2 );
        }
        m_ref2 = luaL_ref( p_L, LUA_REGISTRYINDEX );
    }
    else
    {
		lua_pushstring( p_L, "SetScenegraphEventCallback : bad arg type, function expected" );
		lua_error( p_L );		
    }
    return 0;
}


int LuaDrawSpace::Lua_UpdatePassShaderParam( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 6 )
	{
		lua_pushstring( p_L, "UpdatePassShaderParam : bad number of args" );
		lua_error( p_L );		
	}

    const char* passname = luaL_checkstring( p_L, 1 );
    const char* paramid = luaL_checkstring( p_L, 2 );

    dsreal valx = luaL_checknumber( p_L, 3 );
    dsreal valy = luaL_checknumber( p_L, 4 );
    dsreal valz = luaL_checknumber( p_L, 5 );
    dsreal valw = luaL_checknumber( p_L, 6 );

    if( m_scriptcalls_handler )
    {
        PropertyPool props;
        props.AddPropValue<dsstring>( "script_call_id", "DrawSpace:UpdatePassShaderParam" );
        props.AddPropValue<dsstring>( "passname", passname );
        props.AddPropValue<dsstring>( "paramid", paramid );
        props.AddPropValue<Vector>( "values", Vector( valx, valy, valz, valw ) );

        (*m_scriptcalls_handler)( props );
    }


    return 0;
}

void LuaDrawSpace::on_scenenodegraph_evt( SceneNodeGraph::ScenegraphEvent p_evt, SceneNodeGraph* p_scenegraph )
{
    bool found = false;
    dsstring scenegraph_name;

    for( size_t i = 0; i < m_nodesevent_callbacks.size() && !found; i++ )
    {
        if( m_nodesevent_callbacks[i].scenenodegraph == p_scenegraph )
        {
            found = true;
            scenegraph_name = m_nodesevent_callbacks[i].alias;
        }
    }

    if( found && m_ref2 != -1 )
    {
        // call here the lua callback   
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref2 );

	    lua_pushinteger( m_L, (int)p_evt );
	    lua_pushstring( m_L, scenegraph_name.c_str() );
	    lua_call( m_L, 2, 0 );
    }
}

void LuaDrawSpace::on_scenenode_evt( SceneNodeGraph::NodesEvent p_evt, BaseSceneNode* p_node )
{
    dsstring node_alias;
    p_node->GetSceneName( node_alias );
    dsstring scenegraph_name;
    bool found = false;
    for( size_t i = 0; i < m_nodesevent_callbacks.size() && !found; i++ )
    {
        std::vector<BaseSceneNode*> list = m_nodesevent_callbacks[i].scenenodegraph->GetAllNodesList();
        for( size_t j = 0; j < list.size() && !found; j++ )
        {
            if( list[j] == p_node )
            {
                scenegraph_name = m_nodesevent_callbacks[i].alias;
                found = true;
            }
        }
    }

    if( found && m_ref != -1 )
    {
        // call here the lua callback   
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );

	    lua_pushinteger( m_L, (int)p_evt );
	    lua_pushstring( m_L, scenegraph_name.c_str() );
        lua_pushstring( m_L, node_alias.c_str() );
        	   
	    lua_call( m_L, 3, 0 );
    }
}
