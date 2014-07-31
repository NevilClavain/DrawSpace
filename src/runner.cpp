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

#include "runner.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


Runner::Runner( void )
{

}

Runner::~Runner( void )
{
    
}

void Runner::Run( void )
{
    Mediator* mediator = Mediator::GetInstance();

    while( 1 )
    {
        /*
        Mediator::Event* evt = mediator->Wait();

        if( evt )
        {
            if( m_handlers.count( evt->name ) > 0 )
            {
                MediatorEventHandler* handler = m_handlers[evt->name];
                (*handler)( evt->args );
            }
        }
        */

        Mediator::MessageQueue* queue = mediator->Wait();
        if( queue )
        {
            if( m_handlers.count( queue->m_name ) > 0 )
            {
                MediatorEventHandler* handler = m_handlers[queue->m_name];

                PropertyPool props;

                while( queue->GetNextMessage( props ) )
                {
                    (*handler)( &props );
                }
            }
        }
    }
}

/*
void Runner::RegisterEventHandler( Mediator::Event* p_event, MediatorEventHandler* p_handler )
{
    m_handlers[p_event->name] = p_handler;
}
*/

void Runner::RegisterMsgHandler( Mediator::MessageQueue* p_queue, MediatorEventHandler* p_handler )
{
    m_handlers[p_queue->m_name] = p_handler;
}