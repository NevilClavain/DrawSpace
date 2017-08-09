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


class BaseArguments abstract {};

class Entity
{
private:
    std::unordered_map<int, std::vector<std::unique_ptr<ecs::BaseArguments>>>   m_arguments;

public:

    template<class... Args>
    void RegisterAction(int p_id, const Args&... p_args)
    {              
        m_arguments[p_id].push_back( std::make_unique<ecs::Arguments<Args...>>(p_args...) );
    }

    friend class EntityTree;
};

////////////////////////////System///////////////////////////////////////

class System abstract
{
protected:

    virtual void on_entity_visited_action(int p_actionid, ecs::BaseArguments* p_args) const = 0;
    virtual void on_entity_added_action(int p_actionid, ecs::BaseArguments* p_args) const = 0;

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
                p_system->on_entity_added_action( it->first, it->second[i].get() );
            }            
        }
    }

    void trigger_entity_visited_actions(System* p_system, Entity* p_elt) const
    {
        for (auto it = p_elt->m_arguments.begin(); it != p_elt->m_arguments.end(); ++it)
        {            
            for (size_t i = 0; i < it->second.size(); i++)
            {
                p_system->on_entity_visited_action( it->first, it->second[i].get() );
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
    Arguments(const Args&... p_args) : m_args(p_args...)
    {
        _asm nop
    }
    
    std::tuple<Args...> Get(void) const
    {
        return m_args;
    }
};

struct RGBAColor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    RGBAColor( void ) :
    r( 0 ),
    g( 0 ),
    b( 0 ),
    a( 0 )
    {};

    RGBAColor( unsigned char p_r, unsigned char p_g, unsigned char p_b, unsigned char p_a ) :
    r( p_r ),
    g( p_g ),
    b( p_b ),
    a( p_a )
    {};
};

}

//////////////////////////////////////////////////////////////////////
}

#endif