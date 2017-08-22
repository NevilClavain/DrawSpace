/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _ECS_H_
#define _ECS_H_

#include "drawspace_commons.h"
#include "exceptions.h"
#include "st_tree.h"

namespace DrawSpace
{
namespace ecs
{
///////////////////////////Components/////////////////////////////////////

class BaseComponent abstract {};

template<typename T>
using ComponentPurpose = std::unique_ptr<T>;


template<typename T>
class Component sealed : public BaseComponent
{
private:
    ComponentPurpose<T>       m_purpose;

public:
    template<class... Args>
    void MakePurpose(const Args&... p_args)
    {
        m_purpose = std::make_unique<T>(p_args...);
    }
    
    virtual T& getPurpose(void) const
    {
        return *( m_purpose.get() );
    }    
};

template<typename T>
class MultiPurposeComponent sealed : public BaseComponent
{
private:
    std::vector<ComponentPurpose<T>>       m_purposes;

public:
    template<class... Args>
    void MakePurposes(size_t p_nb, const Args&... p_args)
    {
        for(size_t i = 0; i < p_nb; i++)
        {
            m_purposes.push_back( std::make_unique<T>(p_args...) );
        }
    }

    virtual T& getPurpose(long p_index) const
    {
        return *( m_purposes[p_index].get() );
    }
};

///////////////////////////Entity/////////////////////////////////////

class BaseArguments abstract
{
protected:
    size_t              m_comp_src;
    size_t              m_comp_dst;
public:
    BaseArguments( size_t p_src, size_t p_dst ) : 
    m_comp_src( p_src ),
    m_comp_dst( p_dst )
    {
    }

    virtual size_t GetSrc( void ) const
    {
        return m_comp_src;
    }

    virtual size_t GetDst( void ) const
    {
        return m_comp_dst;
    }
};

class Entity
{
private:
    std::unordered_map<size_t, std::unique_ptr<BaseComponent>>                  m_components;
    std::unordered_map<int, std::vector<std::unique_ptr<ecs::BaseArguments>>>   m_arguments;

public:

    template<typename T>
    void AddComponent( void )
    {
        size_t tid = typeid(T).hash_code();
        if( m_components.count( tid ) )
        {
            _DSEXCEPTION( "Entity cannot support more than one component of same type : " + dsstring( typeid(T).name() ) );
        }
        m_components[tid] = std::make_unique<Component<T>>();
    }

    template<typename T>
    void AddMultiPurposeComponent( void )
    {
        size_t tid = typeid(T).hash_code();
        if( m_components.count( tid ) )
        {
            _DSEXCEPTION( "Entity cannot support more than one component of same type : " + dsstring( typeid(T).name() ) );
        }
        m_components[tid] = std::make_unique<MultiPurposeComponent<T>>();
    }

    template<typename T, class... Args>
    void RegisterSingleComponentAction( int p_id, const Args&... p_args )
    {
        size_t tid = typeid(T).hash_code();
        m_arguments[p_id].push_back( std::make_unique<ecs::Arguments<Args...>>(tid, -1, p_args...) );
    }

    template<typename T, class... Args>
    void UpdateSingleComponentAction( int p_id, int p_index, const Args&... p_args )
    {
        size_t tid = typeid(T).hash_code();
        
        ecs::Arguments<Args...> new_val( tid, -1, p_args... );

        ecs::Arguments<Args...>* arg_ptr = static_cast<ecs::Arguments<Args...>*>( m_arguments[p_id][p_index].get() );
        *arg_ptr = new_val;
    }

    template<typename T1, typename T2, class... Args>
    void RegisterDoubleComponentsAction( int p_id, const Args&... p_args )
    {
        size_t tid_1 = typeid(T1).hash_code();
        size_t tid_2 = typeid(T2).hash_code();

        m_arguments[p_id].push_back( std::make_unique<ecs::Arguments<Args...>>(tid_1, tid_2, p_args...) );
    }

    template<typename T1, typename T2, class... Args>
    void UpdateDoubleComponentAction( int p_id, int p_index, const Args&... p_args )
    {
        size_t tid_1 = typeid(T1).hash_code();
        size_t tid_2 = typeid(T2).hash_code();

        ecs::Arguments<Args...> new_val( tid_1, tid_2, p_args... );

        ecs::Arguments<Args...>* arg_ptr = static_cast<ecs::Arguments<Args...>*>( m_arguments[p_id][p_index].get() );
        *arg_ptr = new_val;
    }

    friend class EntityTree;
};


////////////////////////////System///////////////////////////////////////

class System abstract
{
protected:

    virtual void on_entity_visited_action( int p_actionid, ecs::BaseArguments* p_args, BaseComponent* p_src, BaseComponent* p_dst ) const = 0;
    virtual void on_entity_added_action( int p_actionid, ecs::BaseArguments* p_args, BaseComponent* p_src, BaseComponent* p_dst ) const = 0;

public:
    friend class EntityTree;
};

////////////////////////////Entity tree//////////////////////////////////

class EntityTree
{
private:
    st_tree::tree<Entity*>                                                      m_tr;
    std::unordered_map<dsstring, st_tree::tree<Entity*>::node_type*>            m_nodes; // tout les nodes de m_tr mis a plat...
    
    void trigger_entity_added_actions(System* p_system, Entity* p_elt) const
    {
        for (auto it = p_elt->m_arguments.begin(); it != p_elt->m_arguments.end(); ++it)
        {            
            for (size_t i = 0; i < it->second.size(); i++)
            {
                BaseArguments* args = it->second[i].get();
                size_t comp_src_id = args->GetSrc();
                size_t comp_dst_id = args->GetDst();

                BaseComponent* src_comp = NULL;
                BaseComponent* dst_comp = NULL;

                if( p_elt->m_components.count( comp_src_id ) )
                {
                    src_comp = p_elt->m_components[comp_src_id].get();
                }
                
                if( p_elt->m_components.count( comp_dst_id ) )
                {
                    dst_comp = p_elt->m_components[comp_dst_id].get();
                }

                p_system->on_entity_added_action( it->first, args, src_comp, dst_comp );
            }            
        }
    }

    void trigger_entity_visited_actions(System* p_system, Entity* p_elt) const
    {
        for (auto it = p_elt->m_arguments.begin(); it != p_elt->m_arguments.end(); ++it)
        {            
            for (size_t i = 0; i < it->second.size(); i++)
            {
                BaseArguments* args = it->second[i].get();
                size_t comp_src_id = args->GetSrc();
                size_t comp_dst_id = args->GetDst();

                BaseComponent* src_comp = NULL;
                BaseComponent* dst_comp = NULL;

                if( p_elt->m_components.count( comp_src_id ) )
                {
                    src_comp = p_elt->m_components[comp_src_id].get();
                }
                
                if( p_elt->m_components.count( comp_dst_id ) )
                {
                    dst_comp = p_elt->m_components[comp_dst_id].get();
                }

                p_system->on_entity_visited_action( it->first, args, src_comp, dst_comp );
            }            
        }
    }

public:

    void AddRoot(System* p_system, Entity* p_elt)
    {
        m_tr.insert(p_elt);
        m_nodes["root"] = &m_tr.root();

        trigger_entity_added_actions(p_system, p_elt);
    }

    template <typename... Types>
    void AddLeaf(System* p_system, Entity* p_elt, Types... p_indexes)
    {
        AddLeaf(p_system, p_elt, { p_indexes... });
    }

    void AddLeaf(System* p_system, Entity* p_elt, const std::vector<int>& p_indexes)
    {
        char comment[1024];

        dsstring indexes_sig = "root";
        for (size_t i = 0; i < p_indexes.size(); i++)
        {
            sprintf(comment, ".%d", p_indexes[i]);
            indexes_sig += comment;
        }

        m_nodes[indexes_sig]->insert(p_elt);

        sprintf(comment, ".%d", m_nodes[indexes_sig]->size() - 1);
        dsstring new_indexes_sig = indexes_sig + comment;

        m_nodes[new_indexes_sig] = &(*m_nodes[indexes_sig])[m_nodes[indexes_sig]->size() - 1];

        trigger_entity_added_actions(p_system, p_elt);
    }

    void AcceptSystemTopDownRecursive(System* p_system)
    {
        for (st_tree::tree<Entity*>::df_pre_iterator it2(m_tr.df_pre_begin()); it2 != m_tr.df_pre_end(); ++it2)
        {
            Entity* curr_entity = it2->data();

            trigger_entity_visited_actions(p_system, curr_entity);
        }
    }

    void AcceptSystemLeafsToTopRecursive(System* p_system)
    {
        for (st_tree::tree<Entity*>::df_post_iterator it2(m_tr.df_post_begin()); it2 != m_tr.df_post_end(); ++it2)
        {
            Entity* curr_entity = it2->data();

            trigger_entity_visited_actions(p_system, curr_entity);
        }
    }
};

////////////////Args for relation between components /////////////////

template <typename... Args>
class Arguments sealed : public BaseArguments
{
private:
    std::tuple<Args...> m_args;


public:
    Arguments(size_t p_src, size_t p_dst, const Args&... p_args) : BaseArguments( p_src, p_dst ), 
    m_args(p_args...)
    {
    }
    
    std::tuple<Args...> GetArg(void) const
    {
        return m_args;
    }
};

////////////////////////////////////////////////////////////////////////////

}

//////////////////////////////////////////////////////////////////////
}

#endif