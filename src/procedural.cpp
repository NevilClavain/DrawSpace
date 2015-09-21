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
#include "misc_utils.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Procedural;
using namespace DrawSpace::Utils;

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

bool RootParser::Parse( long p_line_num, std::vector<dsstring>& p_words, std::stack<std::pair<OpcodeParser*, Atomic*>>& p_stack )
{
    return true;
}

void RootParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child )
{
    m_rules = p_child;
}

Atomic* RootParser::GetRules( void )
{
    return m_rules;
}

PubParser::PubParser( DrawSpace::Core::BaseCallback<void, Atomic*>* p_handler ) : 
m_handler( p_handler )
{
}

PubParser::~PubParser( void )
{
}

bool PubParser::Parse( long p_line_num, std::vector<dsstring>& p_words, std::stack<std::pair<OpcodeParser*, Atomic*>>& p_stack )
{
    Publisher* pub = _DRAWSPACE_NEW_( Publisher, Publisher );
    pub->RegisterHandler( m_handler );

    OpcodeParser* parent_parser = p_stack.top().first;
    parent_parser->SubmitAtomic( p_stack.top().second, pub );

    p_stack.push( std::pair<OpcodeParser*, Atomic*>( this, pub ) );

    return true;
}

void PubParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child )
{
    // get associated publisher
    Publisher* pub = static_cast<Publisher*>( p_parent );
    pub->SetChild( p_child );
}


bool IntegerParser::Parse( long p_line_num, std::vector<dsstring>& p_words, std::stack<std::pair<OpcodeParser*, Atomic*>>& p_stack )
{
    if( p_words.size() < 2 )
    {        
        return false;
    }

    Integer* integer = _DRAWSPACE_NEW_( Integer, Integer );
    integer->SetValue( StringToInt( p_words[1] ) );

    OpcodeParser* parent_parser = p_stack.top().first;
    parent_parser->SubmitAtomic( p_stack.top().second, integer );

    return true;
}

void IntegerParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child )
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RulesPackage::RulesPackage( DrawSpace::Core::BaseCallback<void, Atomic*>* p_handler )
{
    m_root = _DRAWSPACE_NEW_( RootParser, RootParser );
    m_stack.push( std::pair<OpcodeParser*, Atomic*>( m_root, NULL ) );
    m_opcodes["pub"] = _DRAWSPACE_NEW_( PubParser, PubParser( p_handler ) );
    m_opcodes["integer"] = _DRAWSPACE_NEW_( IntegerParser, IntegerParser );
}
    
RulesPackage::~RulesPackage( void )
{
}

bool RulesPackage::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "end" == p_line )
    {
        m_stack.pop();
    }
    else
    {
        if( m_opcodes.count( p_words[0] ) > 0 )
        {            
            if( false == m_opcodes[p_words[0]]->Parse( p_line_num, p_words, m_stack ) )
            {
                _PARSER_MISSING_ARG__
                return false;
            }            
        }
        else
        {            
            _PARSER_UNEXPECTED_KEYWORD_
            return false;
        }
    }
    return true;
}

RootParser* RulesPackage::GetRootParser( void )
{
    return m_root;
}