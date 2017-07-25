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

#ifndef _ENTITYSET_H_
#define _ENTITYSET_H_

#include "tree_container.h"
#include <vector>
#include "drawspace_commons.h"

namespace DrawSpace
{


using EntitiesTree = st_tree::tree<Entity*>;


class EntitySet
{
private:

    std::vector<EntitiesTree>           m_entities;

protected:
    std::vector<EntityTreeContainer*>   m_entities_containers;


public:

    typedef enum
    {
        PHASE_INIT,
        PHASE_RELEASE,
        PHASE_RUN

    } Phase;

    EntitySet( void );
    ~EntitySet( void );

    //void InsertTree( const st_tree::tree<Entity*>& p_tree );

    void AcceptSystemTopDownRecursive( Interface::System* p_system, Phase p_phase );
    void AcceptSystemLeafsToTopRecursive( Interface::System* p_system, Phase p_phase );

    virtual void AddRoot( Entity* p_elt );

    template <typename... Types>
    void AddLeaf( Entity* p_elt, int p_root_index, Types... p_indexes )
    {        
        m_entities_containers[p_root_index].AddLeaf( p_elt, p_indexes...);
    }

};
}

#endif
