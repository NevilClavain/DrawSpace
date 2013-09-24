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

#include "mutex.h"
#include "tracedefs.h"
_DECLARE_DS_LOGGER( logger, "Mutex" )

using namespace DrawSpace;

Utils::Mutex::Mutex( void )
{
    m_hmutex = CreateMutex( NULL, FALSE, NULL );
    _DSDEBUG( logger, "handle=" << m_hmutex )
}

Utils::Mutex::~Mutex( void )
{    
    CloseHandle( m_hmutex );
    _DSDEBUG( logger, "closed handle " << m_hmutex )
}

void Utils::Mutex::WaitInfinite( void )
{
    DWORD status = WaitForSingleObject( m_hmutex, INFINITE );

    if( WAIT_OBJECT_0 != status )
    {        
        _DSERROR( logger, "unexpected return for WaitForSingleObject, handle " << m_hmutex )
    }

    _DSTRACE( logger, "getting mutex, handle " << m_hmutex )
}

bool Utils::Mutex::Wait( long p_interval )
{
    bool ret;
    DWORD status = WaitForSingleObject( m_hmutex, p_interval );

    switch( status )
    {
        case WAIT_OBJECT_0:

            _DSTRACE( logger, "getting mutex, handle " << m_hmutex )
            ret = true;
            break;

        case WAIT_TIMEOUT:

            _DSTRACE( logger, "timed out on mutex, handle " << m_hmutex )
            ret = false;
            break;

        default:

            _DSERROR( logger, "unexpected return for WaitForSingleObject, handle " << m_hmutex )
            ret = false;
            break;
    }
    return ret;
}

void Utils::Mutex::Release( void )
{
    ReleaseMutex( m_hmutex );
}