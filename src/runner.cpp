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

#include "runner.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


Runner::Runner( void )
{
    m_task = _DRAWSPACE_NEW_( Task<Runner>, Task<Runner> );
    m_task_message_queue = m_mediator.CreateMessageQueue();
    m_client_message_queue = m_mediator.CreateMessageQueue();
}

Runner::~Runner( void )
{
    _DRAWSPACE_DELETE_( m_task );
}

void Runner::unstack_messages( DrawSpace::Core::Mediator::MessageQueue* p_testqueue, DrawSpace::Core::Mediator::MessageQueue* p_signaledqueue, MediatorEventHandler* p_handler )
{
    if( p_testqueue == p_signaledqueue )
    {
        PropertyPool props;
        while( p_signaledqueue->GetNextMessage( props ) )
        {
            (*p_handler)( &props );
        }
    }    
}

void Runner::Run( void )
{
    while( 1 )
    {
        Mediator::MessageQueue* signaled_queue = m_mediator.Wait();
        unstack_messages( m_task_message_queue, signaled_queue, m_taskhandler );
    }
}

void Runner::Check( void )
{
    Mediator::MessageQueue* signaled_queue = m_mediator.Check();
    unstack_messages( m_client_message_queue, signaled_queue, m_clienthandler );
}

void Runner::RegisterTaskMsgHandler( MediatorEventHandler* p_handler )
{    
    m_taskhandler = p_handler;
}

void Runner::PushMessage( const PropertyPool& p_msg )
{
    m_task_message_queue->PushMessage( p_msg );
}

void Runner::RegisterClientMsgHandler( MediatorEventHandler* p_handler )
{
    m_clienthandler = p_handler;
}

void Runner::Startup( void )
{
    m_task->Startup( this );
}