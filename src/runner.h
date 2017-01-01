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

    typedef enum
    {
        TASK_WAIT,
        TASK_RUNNING,
        TASK_DONE,

    } State;

    typedef DrawSpace::Core::BaseCallback<void, PropertyPool*>      MediatorEventHandler;
    typedef DrawSpace::Core::BaseCallback<void, State>              EventHandler;

protected:

    DrawSpace::Core::Task<DrawSpace::Core::Runner>*                 m_task;
    DrawSpace::Core::Mediator                                       m_mediator;

    DrawSpace::Core::Mediator::MessageQueue*                        m_task_message_queue;
    MediatorEventHandler*                                           m_taskhandler;

    EventHandler*                                                   m_clienthandler;

    DrawSpace::Utils::Mutex                                         m_current_state_mutex;
    State                                                           m_current_state;

    State                                                           m_current_state_client_copy;

    void unstack_messages( DrawSpace::Core::Mediator::MessageQueue* p_testqueue, DrawSpace::Core::Mediator::MessageQueue* p_signaledqueue, MediatorEventHandler* p_handler );

    void update_state( State p_state );

public:
    
    Runner( void );
    virtual ~Runner( void );

    void Run( void );
    void Startup( void );

    void RegisterTaskMsgHandler( MediatorEventHandler* p_handler );
    void PushMessage( const PropertyPool& p_msg );

    void RegisterEventHandler( EventHandler* p_handler );
    void Check( void );   

    void ResetState( void );
};
}
}

#endif
