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
#include "luaclass_rawtransform.h"
#include "luaclass_entity.h"
#include "luaclass_matrix.h"

using namespace DrawSpace::Utils;
using namespace DrawSpace::Maths;
using namespace DrawSpace::Aspect;

const char LuaClass_RawTransform::className[] = "RawTransform";
const Luna<LuaClass_RawTransform>::RegType LuaClass_RawTransform::methods[] =
{
    { "configure", &LuaClass_RawTransform::LUA_configure },
    { "add_matrix", &LuaClass_RawTransform::LUA_addmatrix },
    { "update_matrix", &LuaClass_RawTransform::LUA_updatematrix },
    { "release", &LuaClass_RawTransform::LUA_release },
	{ 0, 0 }
};


LuaClass_RawTransform::LuaClass_RawTransform( lua_State* p_L ) :
m_entity_transform_aspect( NULL )
{
}

LuaClass_RawTransform::~LuaClass_RawTransform( void )
{
}

int LuaClass_RawTransform::LUA_configure( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
        LUA_ERROR( "RawTransform::configure : argument(s) missing" );
	}

    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 1 );
    int transfoimpl_order = luaL_checkint(p_L, 2);

    DrawSpace::Core::Entity& entity = lua_ent->GetEntity();

    // recupere l'aspect transfo s'il existe pour cette entitee
    TransformAspect* transform_aspect = entity.GetAspect<TransformAspect>();
    if( transform_aspect )
    {
        transform_aspect->AddImplementation(transfoimpl_order, &m_raw_transformer);
        m_entity_transform_aspect = transform_aspect;
    }
    else
    {
        m_entity_transform_aspect = NULL;
        LUA_ERROR( "RawTransform::configure : entity passed on arg has no transform aspect" );
    }

    return 0;
}

int LuaClass_RawTransform::LUA_release( lua_State* p_L )
{
    if( !m_entity_transform_aspect )
    {
        LUA_ERROR( "RawTransform::release : no transform aspect" );
    }

    // clear all matrix components
    for( size_t i = 0; i < m_matrix_ids.size(); i++ )
    {        
        LUA_TRY
        {
            m_entity_transform_aspect->RemoveComponent<Matrix>( m_matrix_ids[i] );

        } LUA_CATCH;
    }

    //m_entity_transform_aspect->RemoveImplementation();
    m_entity_transform_aspect->RemoveAllImplementations();
    m_entity_transform_aspect = NULL;
	m_matrix_ids.clear();
    return 0;
}

int LuaClass_RawTransform::LUA_addmatrix( lua_State* p_L )
{
    if( !m_entity_transform_aspect )
    {
        LUA_ERROR( "RawTransform::add_matrix : no transform aspect" );
    }

	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
        LUA_ERROR( "RawTransform::add_matrix : argument(s) missing" );
	}

    dsstring id = luaL_checkstring( p_L, 1 );
    LuaClass_Matrix* lua_mat = Luna<LuaClass_Matrix>::check( p_L, 2 );

    LUA_TRY
    {
        m_entity_transform_aspect->AddComponent<Matrix>( id, lua_mat->GetMatrix() );

    } LUA_CATCH;

    m_matrix_ids.push_back( id ); // memoriser pour le release()

    return 0;
}

int LuaClass_RawTransform::LUA_updatematrix( lua_State* p_L )
{
    if( !m_entity_transform_aspect )
    {
        LUA_ERROR( "RawTransform::update_matrix : no transform aspect" );
    }

	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{
        LUA_ERROR( "RawTransform::update_matrix : argument(s) missing" );
	}

    dsstring id = luaL_checkstring( p_L, 1 );
    LuaClass_Matrix* lua_mat = Luna<LuaClass_Matrix>::check( p_L, 2 );

    LUA_TRY
    {
        m_entity_transform_aspect->GetComponent<Matrix>( id )->getPurpose() = lua_mat->GetMatrix();

    } LUA_CATCH;

    return 0;
}

