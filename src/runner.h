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
#include "task.h"

namespace DrawSpace
{
namespace Core
{
class Runner
{
public:
    typedef DrawSpace::Core::BaseCallback<void, PropertyPool*>      MediatorEventHandler;

protected:

    DrawSpace::Core::Task<DrawSpace::Core::Runner>*                 m_task;
    DrawSpace::Core::Mediator                                       m_mediator;

    DrawSpace::Core::Mediator::MessageQueue*                        m_task_message_queue;
    std::map<HANDLE, MediatorEventHandler*>                         m_taskhandlers;

    DrawSpace::Core::Mediator::MessageQueue*                        m_client_message_queue;
    std::map<HANDLE, MediatorEventHandler*>                         m_clienthandlers;


public:
    
    Runner( void );
    virtual ~Runner( void );

    void Run( void );

    void Startup( void );
    void RegisterTaskMsgHandler( MediatorEventHandler* p_handler );
    void PushMessage( const PropertyPool& p_msg );

    void RegisterClientMsgHandler( MediatorEventHandler* p_handler );

};
}
}

#endif
