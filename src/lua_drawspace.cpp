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

#include "lua_drawspace.h"
#include "lua_real.h"
#include "luacontext.h"
#include "lua_camerapointnode.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaDrawSpace::className[] = "DrawSpace";
const Luna2<LuaDrawSpace>::RegType LuaDrawSpace::methods[] =
{
  { "DisplayFramerate", &LuaDrawSpace::Lua_DisplayFramerate },
  { "DisplayCurrentCamera", &LuaDrawSpace::Lua_DisplayCurrentCamera },
  { "CreateSceneNodeGraph", &LuaDrawSpace::Lua_CreateSceneNodeGraph },
  { "SetSceneNodeGraphCurrentCamera", &LuaDrawSpace::Lua_SetSceneNodeGraphCurrentCamera },
  { "LoadKeyUpScript", &LuaDrawSpace::Lua_LoadKeyUpScript },
  { "LoadKeyDownScript", &LuaDrawSpace::Lua_LoadKeyDownScript },
  { "LoadMouseScript", &LuaDrawSpace::Lua_LoadMouseScript },
  { "AngleSpeedInc", &LuaDrawSpace::Lua_AngleSpeedInc },
  { "AngleSpeedDec", &LuaDrawSpace::Lua_AngleSpeedDec },
  { "TranslationSpeedInc", &LuaDrawSpace::Lua_TranslationSpeedInc },
  { "TranslationSpeedDec", &LuaDrawSpace::Lua_TranslationSpeedDec },
  { "GetSceneCameraName", &LuaDrawSpace::Lua_GetSceneCameraName },
  { "IsCurrentCamera", &LuaDrawSpace::Lua_IsCurrentCamera },
  { 0 }
};


LuaDrawSpace::LuaDrawSpace( lua_State* p_L ) :
m_timer( NULL )
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

        new_sc_descr.cb = new NodesEventCallback( this, &LuaDrawSpace::on_scenenodegraph_evt );

        new_sc_descr.scenenodegraph->RegisterNodesEvtHandler( new_sc_descr.cb );

        m_nodesevent_callbacks.push_back( new_sc_descr );
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

void LuaDrawSpace::on_scenenodegraph_evt( DrawSpace::Core::SceneNodeGraph::NodesEvent p_evt, DrawSpace::Core::BaseSceneNode* p_node )
{
    // call here the lua callback
    _asm nop
}