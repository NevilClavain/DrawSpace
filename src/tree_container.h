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

#ifndef _TREE_CONTAINER_H_
#define _TREE_CONTAINER_H_

#include "st_tree.h"
#include "entity.h"

namespace DrawSpace
{
namespace Interface
{
class System;
}

class EntityTreeContainer
{
private:
    
    st_tree::tree<Entity*>                                        m_tr;
    std::map<dsstring, st_tree::tree<Entity*>::node_type*>        m_nodes; // tout les nodes de m_tr mis a plat...

public:
    
    void AddRoot( Entity* p_elt );

    template <typename... Types>
    void AddLeaf( Entity* p_elt, Types... p_indexes )
    {        
        AddLeaf( p_elt, { p_indexes... } );
    }

    void AddLeaf( Entity* p_elt, const std::vector<int> p_indexes );

    void AcceptSystemTopDownRecursive( Interface::System* p_system );
    void AcceptSystemLeafsToTopRecursive( Interface::System* p_system );
};

}

#endif