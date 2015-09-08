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

#include "procedural.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Procedural;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Atomic::GetId( dsstring& p_id )
{
    p_id = m_id;
}

void Atomic::SetId( const dsstring& p_id )
{
    m_id = p_id;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Publisher::Publisher( void ) : m_child( NULL ), m_handler( NULL )
{
}

void Publisher::Apply( void )
{
    if( m_handler && m_child )
    {
        m_child->Apply();
        (*m_handler)( m_child->GetResultValue() );
    }
}

Atomic* Publisher::GetResultValue( void )
{
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UniformDistributionRandom::UniformDistributionRandom( int p_seed, int p_min, int p_max )
{
    m_generator = _DRAWSPACE_NEW_( std::default_random_engine, std::default_random_engine );
    m_distribution = _DRAWSPACE_NEW_( std::uniform_int_distribution<int>,std::uniform_int_distribution<int>( p_min, p_max ) );
    m_generator->seed( p_seed );
}

UniformDistributionRandom::~UniformDistributionRandom( void )
{
    _DRAWSPACE_DELETE_( m_generator );
    _DRAWSPACE_DELETE_( m_distribution );
}

void UniformDistributionRandom::Apply( void )
{
    m_integer.SetValue( (*m_distribution)( *m_generator ) );
}
Atomic* UniformDistributionRandom::GetResultValue( void )
{
    return &m_integer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Repeat::Repeat( void ) : m_child( NULL ), m_nbloops( NULL )
{
    
}
Repeat::~Repeat( void )
{
}
void Repeat::Apply( void )
{
    if( m_child && m_nbloops )
    {
        Integer* result = dynamic_cast<Integer*>( m_nbloops->GetResultValue() );

        if( result )
        {
            for( int i = 0; i < result->GetValue(); i++ )
            {
                m_child->Apply();
            }
        }
        else
        {
            _DSEXCEPTION( "Procedural Repeat bloc " << m_id << ": loop child result not of Integer type" );
        }
    }
}
Atomic* Repeat::GetResultValue( void )
{
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

