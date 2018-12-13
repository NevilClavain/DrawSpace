/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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


#ifndef _ASPECT_H_
#define _ASPECT_H_

#include "component.h"
#include "exceptions.h"
#include "memalloc.h"

namespace DrawSpace
{
namespace EntityGraph
{
    class EntityNodeGraph;
}

namespace Core
{
class Entity;

template<typename T>
using ComponentList = std::vector<Component<T>*>;

class Aspect abstract
{
protected:

    // map globale, regroupant les composants par id...
    std::unordered_map<dsstring, BaseComponent*>              m_components;

    // 2eme map pour regrouper les composants en fct de leur type
    std::unordered_map<size_t, std::vector<BaseComponent*>>   m_components_by_type;

    // entity owner
    Entity* m_owner;

public:
    Aspect( void ) : m_owner(NULL) {};
    virtual ~Aspect( void ) {}
      
    template<typename T, class... Args>
    inline void AddComponent( const dsstring& p_id, Args&&... p_args )
    {
        if( m_components.count( p_id ) > 0 )
        {
            _DSEXCEPTION( "Component with same id already exists : " + p_id );
        }

        Component<T>* newcomp =  _DRAWSPACE_NEW_( Component<T>, Component<T> );
        newcomp->MakePurpose( (std::forward<Args>(p_args))... );
        m_components[p_id] = newcomp;

        // ajout dans m_components_by_type

        size_t tid = typeid(T).hash_code();
        m_components_by_type[tid].push_back( newcomp );
    }
        
    template<typename T>
    inline void RemoveComponent( const dsstring& p_id )
    {
        if( 0 == m_components.count( p_id ) )
        {
            _DSEXCEPTION( "Component id not registered in this aspect : " + p_id );
        }
        Component<T>* comp = static_cast<Component<T>*>( m_components[p_id] );
              
        // suppression dans m_components_by_type
        size_t tid = typeid(T).hash_code();
        for( auto it = m_components_by_type[tid].begin(); it != m_components_by_type[tid].end(); ++it )
        {
            if( m_components[p_id] == *it )
            {
                // on a trouve le composant en question ! suppression...
                m_components_by_type[tid].erase( it );
                break;
            }
        }
        m_components.erase( p_id );
        //
        _DRAWSPACE_DELETE_( comp );
    }
    
    template<typename T>
    inline Component<T>* GetComponent( const dsstring& p_id )
    {
        if( 0 == m_components.count( p_id ) )
        {
            _DSEXCEPTION( "Component id not registered in this aspect : " + p_id );
        }

        Component<T>* comp = static_cast<Component<T>*>( m_components[p_id] );
        
        return comp;
    }

    template<typename T>
    inline void GetComponentsByType( ComponentList<T>& p_outlist )
    {
        size_t tid = typeid(T).hash_code();
        if( m_components_by_type.count( tid ) > 0 )
        {
            std::vector<BaseComponent*> list = m_components_by_type[tid];
            for( size_t i = 0; i < list.size(); i++ )
            {
                p_outlist.push_back( static_cast<Component<T>*>( list[i] ) );
            }
        }
    }

    inline void SetOwnerEntity(Entity* p_entity) { m_owner = p_entity; }

    virtual void OnAddedInGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph) {};
    virtual void OnRemovedFromGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph) {};
};
}
}

#endif
