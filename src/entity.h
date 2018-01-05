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

#include "aspect.h"

#ifndef _ENTITY_H_
#define _ENTITY_H_

namespace DrawSpace
{
namespace Core
{
class Entity sealed
{
private:
    std::unordered_map<size_t, Aspect*> m_aspects;

public:

    Entity( void ) {};
    ~Entity( void )
    {
        for( auto it = m_aspects.begin(); it != m_aspects.end(); ++it )
        {
            _DRAWSPACE_DELETE_( it->second );
        }
    }
    
    template<typename T>
    T* AddAspect( void )
    {
        if( m_aspects.count(typeid(T).hash_code() ) )
        {
            _DSEXCEPTION( "Aspect type already exists in this entity : " + dsstring( typeid(T).name() ) );
        }
        T* p = _DRAWSPACE_NEW_( T, T );
        m_aspects[typeid(T).hash_code()] = p;
        return p;
    }

    template<typename T>
    T* GetAspect( void )
    {
        size_t tid = typeid(T).hash_code();

        if( 0 == m_aspects.count( tid ) )
        {
            //_DSEXCEPTION( "Aspect type not registered in this entity : " + dsstring( typeid(T).name() ) );
            return NULL;
        }
        
        T* aspect = static_cast<T*>( m_aspects[tid] );
        return aspect;
    }

    void GetAllAspects( std::vector<Aspect*>& p_list )
    {
        for( auto it = m_aspects.begin(); it != m_aspects.end(); ++it )
        {
            p_list.push_back( it->second );
        }
    }
};
}
}


#endif
