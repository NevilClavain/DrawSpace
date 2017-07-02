#include "entityset.h"
#include "system.h"
using namespace DrawSpace;

void EntitySet::InsertTree( const st_tree::tree<Entity*>& p_tree )
{
    m_entities.push_back( p_tree );
}

void EntitySet::AcceptSystemTopDownRecursive( System* p_system )
{
    for( auto it = m_entities.begin(); it != m_entities.end(); ++it )
    {
        st_tree::tree<Entity*> current_tree = *it;
                       
        for( st_tree::tree<Entity*>::df_pre_iterator it2(current_tree.df_pre_begin() );
            it2 != current_tree.df_pre_end(); ++it2 )
        {
            Entity* curr_entity = it2->data();

            p_system->VisitEntitySet( curr_entity);
        }  
    }
}