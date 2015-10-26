/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

    std::map<HANDLE, MediatorEventHandler*>                         m_handlers; 

public:
    
    Runner( void );
    virtual ~Runner( void );

    void Run( void );   
    void RegisterMsgHandler( DrawSpace::Core::Mediator::MessageQueue* p_queue, MediatorEventHandler* p_handler );
};
}
}

#endif
