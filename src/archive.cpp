/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#include "archive.h"
#include "memalloc.h"

using namespace DrawSpace::Utils;

Archive::Archive( void ) : 
m_total_length( 0 ),
m_current_length( 0 ),
m_data( NULL ),
m_current( NULL )
{
}

Archive::Archive( Archive &p_tocopy )
{
    m_total_length = p_tocopy.m_total_length;
    m_current_length = p_tocopy.m_current_length;

    m_data = _DRAWSPACE_NEW_EXPLICIT_SIZE_( unsigned char, unsigned char[m_total_length], m_total_length );

    memcpy( m_data, p_tocopy.m_data, p_tocopy.m_current_length );
    m_current = m_data;
    m_current += p_tocopy.m_current_length;
}

Archive::~Archive( void )
{
    if( m_data )
    {
        _DRAWSPACE_DELETE_N_( m_data );
        m_data = NULL;
    }
}

void Archive::SetArchiveTotalLength( unsigned long p_len )
{
    if( m_data )
    {
        _DRAWSPACE_DELETE_N_( m_data );
        m_data = NULL;
    }

    m_data = _DRAWSPACE_NEW_EXPLICIT_SIZE_( unsigned char, unsigned char[p_len], p_len );

    m_total_length = p_len;
    m_current_length = 0;
    m_current = m_data;
}

long Archive::GetTotalLength( void )
{
    return m_total_length;
}

long Archive::GetCurrentLength( void )
{
    return m_current_length;
}

void Archive::Rewind( void )
{
    m_current = m_data;
    m_current_length = 0;
}

void Archive::operator <<( long p_dword )
{
    memcpy( m_current, &p_dword, sizeof( long ) );
    m_current += sizeof( long );
    m_current_length += sizeof( long );
}

void Archive::operator <<( dsreal p_real )
{
    memcpy( m_current, &p_real, sizeof( dsreal ) );
    m_current += sizeof( dsreal );
    m_current_length += sizeof( dsreal );
}

void Archive::operator <<( dsstring p_string )
{
    const char *str = p_string.c_str();

    memcpy( m_current, str, strlen( str ) );
    m_current += strlen( str );
    *m_current = '\0';
    m_current ++;
    m_current_length += (long)strlen( str ) + 1;
}

void Archive::operator <<( Archive& p_arc )
{
    memcpy( m_current, p_arc.m_data, p_arc.m_current_length );
    m_current += p_arc.m_current_length;
    m_current_length += p_arc.m_current_length;
}

void Archive::operator >>( long& p_dword )
{
    memcpy( &p_dword, m_current, sizeof( long ) );
    m_current += sizeof( long );
}

void Archive::operator >>( dsreal& p_real )
{
    memcpy( &p_real, m_current, sizeof( dsreal ) );
    m_current += sizeof( dsreal );
}

void Archive::operator >>( dsstring& p_string )
{
    p_string = (char *)m_current;
    m_current += strlen( (char *)m_current ) + 1;	
}

unsigned char Archive::operator[]( unsigned long p_index )
{
    return m_data[p_index];
}

unsigned char* Archive::GetCurrentPtr( void )
{
    return m_current;
}
