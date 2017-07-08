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

#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <set>
#include "component.h"

#define CHECK_DS_COMPONENT( __entity__, __comp_type__ ) __entity__->m_components.count( __comp_type__ )
#define EXTRACT_DS_COMPONENT(  __entity__, __comp_decl__, __comp_type__, __index__ ) static_cast<__comp_decl__*>( __entity__->m_components[__comp_type__][__index__] );

namespace DrawSpace
{



class Entity
{
private:

public:
    std::map<ComponentType, std::vector<ComponentBase*>> m_components;

    void operator+= (ComponentBase* p_comp)
    {
        m_components[p_comp->m_type].push_back( p_comp );
    }
};




}

#endif