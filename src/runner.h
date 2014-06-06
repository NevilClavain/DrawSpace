/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#ifndef _RUNNER_H_
#define _RUNNER_H_

#include "mediator.h"
#include "callback.h"

namespace DrawSpace
{
namespace Core
{
class Runner
{
public:

    typedef DrawSpace::Core::BaseCallback<void, PropertyPool*>      MediatorEventHandler;

protected:

    std::map<dsstring, MediatorEventHandler*>                       m_handlers;

    PropertyPool                                                    m_propertypool;

public:
    
    Runner( void );
    virtual ~Runner( void );

    void Run( void );
    void RegisterEventHandler( const dsstring& p_event_name, MediatorEventHandler* p_handler );

    PropertyPool* GetPropertyPool( void );

};
}
}

#endif