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


Runner::Runner( void ) :
m_current_state( TASK_WAIT ),
m_current_state_client_copy( TASK_WAIT ),
m_taskhandler( NULL ),
m_clienthandler( NULL )
{
    m_task = _DRAWSPACE_NEW_( Task<Runner>, Task<Runner> );
    m_task_message_queue = m_mediator.CreateMessageQueue();
}

Runner::~Runner( void )
{
    _DRAWSPACE_DELETE_( m_task );
}

void Runner::unstack_messages( DrawSpace::Core::Mediator::MessageQueue* p_testqueue, DrawSpace::Core::Mediator::MessageQueue* p_signaledqueue, MediatorEventHandler* p_handler )
{
    if( p_testqueue == p_signaledqueue )
    {
        update_state( TASK_RUNNING );
        PropertyPool props;        
        while( p_signaledqueue->GetNextMessage( props ) )
        {
            if( p_handler )
            {
                
                (*p_handler)( &props );                
            }
        }
        update_state( TASK_DONE );
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
    m_current_state_mutex.WaitInfinite();
    State current_state = m_current_state;
    m_current_state_mutex.Release();

    if( m_current_state_client_copy != current_state && m_clienthandler )
    {
        m_current_state_client_copy = current_state;
        (*m_clienthandler)( m_current_state_client_copy );
    }
}

void Runner::RegisterTaskMsgHandler( MediatorEventHandler* p_handler )
{    
    m_taskhandler = p_handler;
}

void Runner::PushMessage( const PropertyPool& p_msg )
{
    m_current_state_mutex.WaitInfinite();
    State current_state = m_current_state;
    m_current_state_mutex.Release();
    
    if( current_state != TASK_WAIT )
    {
        return;
    }
    m_task_message_queue->PushMessage( p_msg );
}


void Runner::RegisterEventHandler( EventHandler* p_handler )
{
    m_clienthandler = p_handler;
}

void Runner::Startup( void )
{
    ResetState();
    m_task->Startup( this );
}

void Runner::update_state( State p_state )
{
    m_current_state_mutex.WaitInfinite();
    m_current_state = p_state;
    m_current_state_mutex.Release();
}

void Runner::ResetState( void )
{
    update_state( TASK_WAIT );
}