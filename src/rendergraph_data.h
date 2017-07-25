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

#ifndef _RENDERGRAPH_DATA_H_
#define _RENDERGRAPH_DATA_H_

#include "entityset.h"

namespace DrawSpace
{

class RenderGraphData : public EntitySet
{
protected:
    void initialize_new_entity( Entity* p_elt );

public:

    void AddRoot( Entity* p_entity );

    template <typename... Types>
    void AddLeaf( Entity* p_elt, int p_root_index, Types... p_indexes )
    {
        initialize_new_entity( p_elt );
        m_entities_containers[p_root_index]->AddLeaf( p_elt, p_indexes...);
    }
};
}
#endif