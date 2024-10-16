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

#include "componentcontainer.h"

#pragma once

namespace DrawSpace
{
namespace Core
{
class Entity
{
private:
    std::unordered_map<size_t, ComponentContainer*> m_aspects;

public:

    Entity( void ) {};
    ~Entity( void )
    {        
        for( auto&e : m_aspects)
        {
            _DRAWSPACE_DELETE_(e.second);
        }
    }
    
    template<typename T>
    inline T* AddAspect( void )
    {
        size_t tid = typeid(T).hash_code();
        if( m_aspects.count( tid ) )
        {
            _DSEXCEPTION( "Aspect type already exists in this entity : " + dsstring( typeid(T).name() ) );
        }
        T* p = _DRAWSPACE_NEW_( T, T );
        p->SetOwnerEntity(this);
        m_aspects[tid] = p;
        return p;
    }

    template<typename T>
    inline void RemoveAspect( void )
    {
        size_t tid = typeid(T).hash_code();
        if( m_aspects.count( tid ) )
        {
            _DRAWSPACE_DELETE_( m_aspects[tid] );
            m_aspects.erase( tid );
        }
        else
        {
            _DSEXCEPTION( "Aspect type doesnt exists in this entity : " + dsstring( typeid(T).name() ) );
        }
    }

    template<typename T>
    inline T* GetAspect( void ) const
    {
        size_t tid = typeid(T).hash_code();

        if( 0 == m_aspects.count( tid ) )
        {
            //_DSEXCEPTION( "Aspect type not registered in this entity : " + dsstring( typeid(T).name() ) );
            return NULL;
        }
        
        T* aspect = static_cast<T*>( m_aspects.at(tid) );
        return aspect;
    }

    inline void GetAllAspects( std::vector<ComponentContainer*>& p_list ) const
    {
        for( auto& e: m_aspects)
        {
            p_list.push_back(e.second);
        }
    }

    inline void OnAddedInGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph, Entity* p_parent_entity)
    {
        for (auto& e : m_aspects)
        {
            e.second->OnAddedInGraph(p_entitynodegraph, p_parent_entity);
        }
    };

    inline void OnRemovedFromGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph, Entity* p_parent_entity)
    {
        for (auto& e : m_aspects)
        {
            e.second->OnRemovedFromGraph(p_entitynodegraph, p_parent_entity);
        }
    };
};
}
}

