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

// Strongly inspired from this : http://accu.org/index.php/journals/562

#ifndef _TASK_H_
#define _TASK_H_

#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Threading
{

template<typename base>
class Task
{
public:
    enum TerminateAction
    {
        Block, //halt in destructor until thread terminates
        Kill,  //terminate thread
        Orphan //orphan thread
    };    

protected:

    HANDLE              m_thread_handle;
    DWORD               m_thread_id;
    //int             m_suspended_count;
    TerminateAction     m_terminate_action;
    
public:

    Task( void ) : m_thread_handle( NULL ), m_thread_id( -1 ), /*m_suspended_count( 0 ),*/ m_terminate_action( Block )
    {
    }
    Task( TerminateAction p_terminate_action ) : m_thread_handle( NULL ), m_thread_id( -1 ), /*m_suspended_count( 0 ),*/ m_terminate_action( p_terminate_action )
    {
    }

    ~Task( void )
    {
        switch( m_terminate_action )
        {
            case Block:

                WaitCompletion();
                break;

            case Kill:
                // force thread closing (it's bad)
                if( !IsTerminated() ) 
                {
                    TerminateThread( m_thread_handle, 0 );
                }
                break;

            case Orphan: // do nothin'

            default:
                break;
        }
    }

    static DWORD WINAPI StartRun( LPVOID p_param )
    {
        base* object = static_cast<base*>( p_param );
        object->Run();
        return 0;
    }

    void Startup( base* p_instance )
    {
        if( m_thread_handle != NULL )
        {
            // don't wanna launch multiple thread on the same object instance
            return;
        }

        m_thread_handle = CreateThread(  NULL, 0, StartRun, p_instance, 0, &m_thread_id );
        /*
        if( NULL == m_thread_handle ) 
        {
            
        }
        */
        
    }

    bool IsTerminated( void ) 
    {
        if( NULL == m_thread_handle ) 
        {       
            return true;
        }

        DWORD w = WaitForSingleObject( m_thread_handle, 0 ); // non -blocking call

        if( WAIT_OBJECT_0 == w )
        {
            m_thread_handle = NULL;
            //m_suspended_count = 0;
            return true;
        }
        return false;
    }
    /*
    bool IsSuspended() 
    {
        return m_suspended_count > 0;
    }*/
/*
    void SuspendThread() 
    {
        if( NULL != m_thread_handle )
        {
            // nothing to suspend
            return;
        }
        m_suspended_count = ::SuspendThread( m_thread_handle );
    }

    void ResumeThread() 
    {
        if( NULL != m_thread_handle )
        {
            // nothing to resume
            return;
        }

        m_suspended_count = ::ResumeThread( m_thread_handle );
    }
*/
    void WaitCompletion( void ) 
    {
        if (!IsTerminated() ) 
        {
            /*
            while( m_suspended_count > 0 )
            {
                ResumeThread();
            }
            */

            WaitForSingleObject( m_thread_handle, INFINITE );
        }
    }
};


}
}

#endif
