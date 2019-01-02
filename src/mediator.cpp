/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#include "mediator.h"
#include "exceptions.h"

using namespace DrawSpace;
//using namespace DrawSpace::Core;
using namespace DrawSpace::Threading;
using namespace DrawSpace::Utils;

Mediator::Mediator( void ) : m_nb_handles( 0 )
{
}


Mediator::~Mediator( void )
{

}

Mediator::MessageQueue* Mediator::CreateMessageQueue( void )
{
    HANDLE evt = ::CreateEventA( NULL, TRUE, FALSE, NULL );
    Mediator::MessageQueue* mq = _DRAWSPACE_NEW_( Mediator::MessageQueue, Mediator::MessageQueue( evt ) );
    
    m_messages_by_handle[evt] = mq;

    m_handles[m_nb_handles] = evt;
    m_nb_handles++;

    return mq;
}

Mediator::MessageQueue* Mediator::Wait( void )
{
    DWORD wait = WaitForMultipleObjects( m_nb_handles, m_handles, false, INFINITE );
    if( WAIT_FAILED == wait )
    {
        _DSEXCEPTION( "unexpected error on WaitForMultipleObjects" )
    }
    wait -= WAIT_OBJECT_0;

    ResetEvent( m_messages_by_handle[m_handles[wait]]->m_system_event );
    return m_messages_by_handle[m_handles[wait]];
}

