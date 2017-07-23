#ifndef _TREECONTAINER_H_
#define _TREECONTAINER_H_

#include "st_tree.h"
#include "entity.h"

namespace DrawSpace
{

class EntityTreeContainer
{
private:
    
    st_tree::tree<Entity*>                                        m_tr;
    std::map<dsstring, st_tree::tree<Entity*>::node_type*>        m_nodes; // tout les nodes de m_tr mis a plat...

public:
    
    void AddRoot( Entity* p_elt )
    {
        m_tr.insert( p_elt );
        m_nodes["root"] = &m_tr.root();
    }

    template <typename... Types>
    void AddLeaf( Entity* p_elt, Types... p_indexes )
    {        
        AddLeaf( p_elt, { p_indexes... } );
    }

    void AddLeaf( Entity* p_elt, const std::vector<int> p_indexes )
    {
        char comment[32];

        dsstring indexes_sig = "root";
        for( size_t i = 0; i < p_indexes.size(); i++ )
        {
            sprintf( comment, ".%d", p_indexes[i] );
            indexes_sig += comment;
        }

        m_nodes[indexes_sig]->insert( p_elt );

        sprintf( comment, ".%d", m_nodes[indexes_sig]->size() - 1 );
        dsstring new_indexes_sig = indexes_sig + comment;

        m_nodes[new_indexes_sig] = &(*m_nodes[indexes_sig])[m_nodes[indexes_sig]->size() - 1];
    }
};

}

#endif