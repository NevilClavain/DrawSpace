/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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
/* -*-LIC_END-*- */

#include "luacontext.h"
#include "luaclass_entitynodegraph.h"
#include "luaclass_entity.h"
#include "mainservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

const char LuaClass_EntityNodeGraph::className[] = "EntityNodeGraph";
const Luna<LuaClass_EntityNodeGraph>::RegType LuaClass_EntityNodeGraph::methods[] =
{
    { "set_root", &LuaClass_EntityNodeGraph::LUA_setroot },
    { "add_child", &LuaClass_EntityNodeGraph::LUA_addchild },
    { "remove", &LuaClass_EntityNodeGraph::LUA_remove },
    { "set_camera", &LuaClass_EntityNodeGraph::LUA_setcamera },
    { "unset_camera", &LuaClass_EntityNodeGraph::LUA_unsetcamera },
    { "dump", &LuaClass_EntityNodeGraph::LUA_dumpcontent },
	{ "find_entityname", &LuaClass_EntityNodeGraph::LUA_findentityname },
    { "register_rigidbody", &LuaClass_EntityNodeGraph::LUA_registerrigidbody },
    { "register_collider", &LuaClass_EntityNodeGraph::LUA_registercollider },
    { "unregister_rigidbody", &LuaClass_EntityNodeGraph::LUA_unregisterrigidbody },
    { "unregister_collider", &LuaClass_EntityNodeGraph::LUA_unregistercollider },

	{ 0, 0 }
};

LuaClass_EntityNodeGraph::LuaClass_EntityNodeGraph( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "EntityNodeGraph::EntityNodeGraph : argument(s) missing" );
	}

    dsstring id = luaL_checkstring( p_L, 1 );
    MainService::GetInstance()->RegisterEntityGraph( id, this );

    m_id = id;
}

LuaClass_EntityNodeGraph::~LuaClass_EntityNodeGraph( void )
{
    MainService::GetInstance()->UnregisterEntityGraph( m_id );
}

DrawSpace::EntityGraph::EntityNode& LuaClass_EntityNodeGraph::GetEntityNode( const dsstring& p_id )
{
    if( m_entities.count( p_id ) )
    {
        return m_entities[p_id];
    }
    else
    {
        _DSEXCEPTION( "Unknown entity" )
    }
}

DrawSpace::EntityGraph::EntityNodeGraph& LuaClass_EntityNodeGraph::GetEntityGraph( void )
{
    return m_entitygraph;
}

int LuaClass_EntityNodeGraph::LUA_setroot( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
        LUA_ERROR( "EntityNodeGraph::set_root : argument(s) missing" );
	}

    dsstring entity_id = luaL_checkstring( p_L, 1 );
    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 2 );
    
    if( m_entities.count( entity_id ) )
    {
        LUA_ERROR( "EntityNodeGraph::set_root : an entity with same id exists in entities table -> " + entity_id);
    }

    m_entities[entity_id] = m_entitygraph.SetRoot( &lua_ent->GetEntity() );

    return 0;
}

int LuaClass_EntityNodeGraph::LUA_addchild( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{
        LUA_ERROR( "EntityNodeGraph::add_child : argument(s) missing" );
	}

    dsstring parent_entity_id = luaL_checkstring( p_L, 1 );
    dsstring entity_id = luaL_checkstring( p_L, 2 );
    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 3 );


    if( m_entities.count( entity_id ) )
    {
        LUA_ERROR( "EntityNodeGraph::add_child : an entity with same id exists in entities table -> " + entity_id);
    }

    if( 0 == m_entities.count( parent_entity_id ) )
    {
        LUA_ERROR( "EntityNodeGraph::add_child : id of parent entity doesnt exists in entities table" );
    }

    m_entities[entity_id] = m_entities[parent_entity_id].AddChild( &lua_ent->GetEntity() );

    return 0;
}

int LuaClass_EntityNodeGraph::LUA_remove( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "EntityNodeGraph::remove : argument(s) missing" );
	}
    dsstring entity_id = luaL_checkstring( p_L, 1 );

    if( 0 == m_entities.count( entity_id ) )
    {
        LUA_ERROR( "EntityNodeGraph::remove : id doesnt exists in entities table: " + entity_id);
    }

    m_entities.at(entity_id).Erase();
    m_entities.erase( entity_id );
    return 0;
}

int LuaClass_EntityNodeGraph::LUA_setcamera( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 1 )
	{
        LUA_ERROR( "EntityNodeGraph::set_camera : argument(s) missing" );
	}

    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 1 );

    m_entitygraph.SetCurrentCameraEntity( &lua_ent->GetEntity() );
    return 0;
}

int LuaClass_EntityNodeGraph::LUA_unsetcamera(lua_State* p_L)
{
    m_entitygraph.SetCurrentCameraEntity(NULL);
    return 0;
}


int LuaClass_EntityNodeGraph::LUA_dumpcontent( lua_State* p_L )
{
    m_entitygraph.PushSignal_DumpToTrace();
    return 0;
}

int LuaClass_EntityNodeGraph::LUA_findentityname(lua_State* p_L)
{
	int argc = lua_gettop(p_L);
	if (argc < 1)
	{
		LUA_ERROR("EntityNodeGraph::find_entityname : argument(s) missing");
	}
	LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check(p_L, 1);
	DrawSpace::Core::Entity& ref_entity = lua_ent->GetEntity();

	const auto entity { std::find_if(m_entities.begin(), m_entities.end(),[&](auto elt) { return(elt.second.GetEntity() == &ref_entity);})};
	if (entity != m_entities.end())
	{
		lua_pushstring(p_L, entity->first.c_str());
		return 1;
	}
	else
	{
		LUA_ERROR("EntityNodeGraph::find_entityname : cannot find entity name : unknow entity");
		return 0;
	}
}

int LuaClass_EntityNodeGraph::LUA_registerrigidbody(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("EntityNodeGraph::register_rigidbody : argument(s) missing");
    }
    LuaClass_Entity* lua_ent{ Luna<LuaClass_Entity>::check(p_L, 1) };
    DrawSpace::Core::Entity& ref_entity{ lua_ent->GetEntity() };

    m_entitygraph.RegisterRigidBody(&ref_entity);

    return 0;
}

int LuaClass_EntityNodeGraph::LUA_registercollider(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("EntityNodeGraph::register_collider : argument(s) missing");
    }
    LuaClass_Entity* lua_ent{ Luna<LuaClass_Entity>::check(p_L, 1) };
    DrawSpace::Core::Entity& ref_entity{ lua_ent->GetEntity() };

    m_entitygraph.RegisterCollider(&ref_entity);

    return 0;
}

int LuaClass_EntityNodeGraph::LUA_unregisterrigidbody(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("EntityNodeGraph::unregister_rigidbody : argument(s) missing");
    }
    LuaClass_Entity* lua_ent{ Luna<LuaClass_Entity>::check(p_L, 1) };
    DrawSpace::Core::Entity& ref_entity{ lua_ent->GetEntity() };

    m_entitygraph.UnregisterRigidBody(&ref_entity);

    return 0;
}

int LuaClass_EntityNodeGraph::LUA_unregistercollider(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("EntityNodeGraph::unregister_collider : argument(s) missing");
    }
    LuaClass_Entity* lua_ent{ Luna<LuaClass_Entity>::check(p_L, 1) };
    DrawSpace::Core::Entity& ref_entity{ lua_ent->GetEntity() };

    m_entitygraph.UnregisterCollider(&ref_entity);

    return 0;
}
