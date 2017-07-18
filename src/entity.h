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


namespace DrawSpace
{
class Entity
{
private:
    std::map<ComponentType, std::vector<ComponentBase*>> m_components;

public:

    int CheckComponent( ComponentType p_type );

    void operator+= (ComponentBase* p_comp);

    template<typename T>
    T* ExtractComponent( ComponentType p_type, int p_index )
    {
        return static_cast<T*>( m_components[p_type][p_index] );
    }
};
}

#endif