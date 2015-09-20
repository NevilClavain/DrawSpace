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


int Array::GetSize( void )
{
    return m_value.size();
}
Atomic* Array::GetValueAt( int p_index )
{
    return m_value[p_index];
}
void Array::AddValue( Atomic* p_value )
{
    m_value.push_back( p_value );
}

void Array::Apply( void )
{
    for( size_t i = 0; i < m_value.size(); i++ )
    {
        m_value[i]->Apply();
    }
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
        m_nbloops->Apply();
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
            _DSEXCEPTION( "Procedural Repeat bloc: loop child result not of Integer type" );
        }
    }
}
Atomic* Repeat::GetResultValue( void )
{
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Batch::Batch( void )
{
}

Batch::~Batch( void )
{
}

void Batch::AddChild( Atomic* p_child )
{
    m_list.push_back( p_child );
}

void Batch::Apply( void )
{
    for( size_t i = 0; i < m_list.size(); i++ )
    {
        m_list[i]->Apply();
    }
}

Atomic* Batch::GetResultValue( void )
{
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Index::Index( void ) : m_array( NULL ), m_index( NULL ), m_result( NULL )
{
}

Index::~Index( void )
{
}

void Index::Apply( void )
{
    if( m_array && m_index )
    {        
        m_index->Apply();
        Integer* index = dynamic_cast<Integer*>( m_index->GetResultValue() );
        if( index )
        {
            m_result = m_array->GetValueAt( index->GetValue() );
        }
        else
        {
            _DSEXCEPTION( "Procedural Index bloc: index child result not of Integer type" );
        }
    }
}

Atomic* Index::GetResultValue( void )
{
    return m_result;
}

void Index::SetIndex( Atomic* p_index )
{
    m_index = p_index;
}

void Index::SetArray( Array* p_array )
{
    m_array = p_array;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RootParser::Parse( const dsstring& p_line, OpcodeParser* p_parent )
{
}

void RootParser::SubmitAtomic( Atomic* p_atomic )
{
    m_rules = p_atomic;
}

void PubParser::Parse( const dsstring& p_line, OpcodeParser* p_parent )
{
    Publisher* pub = _DRAWSPACE_NEW_( Publisher, Publisher );

    // TODO : register pub handler

    if( p_parent )
    {
        p_parent->SubmitAtomic( pub );
    }
}

void PubParser::SubmitAtomic( Atomic* p_atomic )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RulesPackage::RulesPackage( DrawSpace::Core::BaseCallback<void, Atomic*>* p_handler ) :
m_handler( p_handler )
{
    m_currentparser = _DRAWSPACE_NEW_( RootParser, RootParser );
    m_opcodes["pub"] = new PubParser;
}
    
RulesPackage::~RulesPackage( void )
{
}


bool RulesPackage::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "end" == p_line )
    {
    
    }
    else
    {
        if( m_opcodes.count( p_words[0] ) > 0 )
        {
            OpcodeParser* parent = m_currentparser;
            m_currentparser = m_opcodes[p_words[0]];
            m_currentparser->Parse( p_line, parent );
        }
        else
        {
            // error : unknown opcode
            return false;
        }
    }
    return true;
}