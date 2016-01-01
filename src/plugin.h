/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include "drawspace_commons.h"
#include "memalloc.h"

namespace DrawSpace
{
namespace Core
{
template<typename base>
class SingletonPlugin
{
protected:
    static SingletonPlugin* m_instance;

    SingletonPlugin() 
    { 
    };

public:
    base* m_interface;

    ~SingletonPlugin()
    {
    }

    static SingletonPlugin* GetInstance( void )
    {
        if( NULL == m_instance )
        {
            m_instance = _DRAWSPACE_NEW_( SingletonPlugin, SingletonPlugin );
        }
        return m_instance;
    }
};
}
}
#endif
