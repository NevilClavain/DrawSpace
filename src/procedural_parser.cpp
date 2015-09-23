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

#include "procedural_parser.h"
#include "procedural_seeds.h"
#include "exceptions.h"
#include "misc_utils.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Procedural;
using namespace DrawSpace::Utils;

void OpcodeParser::notify_parent( Stack& p_stack, Atomic* p_atomic )
{
    StackEntry se = p_stack.top();
    OpcodeParser* parent_parser = se.parser;

    parent_parser->SubmitAtomic( p_stack.top().atomic, p_atomic, se.arg_counter++ );

    // on update l'element en haut de la pile ( car arg_counter a ete incremente )
    p_stack.pop();
    p_stack.push( se );
}


bool RootParser::Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack )
{
    return true;
}

void RootParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount )
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

bool PubParser::Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack )
{
    Publisher* pub = _DRAWSPACE_NEW_( Publisher, Publisher );
    pub->RegisterHandler( m_handler );

    notify_parent( p_stack, pub );

    StackEntry se;

    se.arg_counter = 0;
    se.parser = this;
    se.atomic = pub;
    p_stack.push( se );

    return true;
}

void PubParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount )
{
    // get associated publisher
    Publisher* pub = static_cast<Publisher*>( p_parent );
    pub->SetChild( p_child );
}


bool IntegerParser::Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack )
{
    if( p_words.size() < 2 )
    {        
        return false;
    }

    Integer* integer = _DRAWSPACE_NEW_( Integer, Integer );
    integer->SetValue( StringToInt( p_words[1] ) );

    notify_parent( p_stack, integer );

    return true;
}

void IntegerParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount )
{
}

bool RealParser::Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack )
{
    if( p_words.size() < 2 )
    {        
        return false;
    }

    Real* real = _DRAWSPACE_NEW_( Real, Real );
    real->SetValue( StringToReal( p_words[1] ) );

    notify_parent( p_stack, real );

    return true;
}

void RealParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount )
{
}

bool StringParser::Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack )
{
    if( p_words.size() < 2 )
    {        
        return false;
    }

    Procedural::String* string = _DRAWSPACE_NEW_( Procedural::String, Procedural::String );
    string->SetValue( p_words[1] );

    notify_parent( p_stack, string );

    return true;
}

void StringParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount )
{
}


bool VectorParser::Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack )
{
    if( p_words.size() < 5 )
    {        
        return false;
    }

    Procedural::Vector* vector = _DRAWSPACE_NEW_( Procedural::Vector, Procedural::Vector );
    vector->SetValue( Utils::Vector( StringToReal( p_words[1] ), StringToReal( p_words[2] ), StringToReal( p_words[3] ), StringToReal( p_words[4] ) ) );

    notify_parent( p_stack, vector );

    return true;
}

void VectorParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount )
{
}


bool ArrayParser::Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack )
{
    Array* array = _DRAWSPACE_NEW_( Array, Array );
    
    notify_parent( p_stack, array );

    //p_stack.push( std::pair<OpcodeParser*, Atomic*>( this, array ) );
    StackEntry se;
    se.arg_counter = 0;
    se.parser = this;
    se.atomic = array;
    p_stack.push( se );

    return true;
}

void ArrayParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount )
{
    // get associated array
    Array* array = static_cast<Array*>( p_parent );
    array->AddValue( p_child );
}


bool RandomDistributionParser::Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack )
{
    Atomic* random_source;

    if( p_words.size() < 6 )
    {        
        return false;
    }

    if( "uniform" == p_words[1] )
    {
        if( "integer" == p_words[2] )
        {
            int sub_seed = SeedsBase::GetInstance()->GetSeed( StringToInt( p_words[5] ) );

            std::uniform_int_distribution<int>* source = _DRAWSPACE_NEW_( std::uniform_int_distribution<int>, std::uniform_int_distribution<int>( StringToInt( p_words[3] ), StringToInt( p_words[4] ) ) );
            UniformIntegerRandom* random = _DRAWSPACE_NEW_( UniformIntegerRandom, UniformIntegerRandom( source, sub_seed ) );

            random_source = random;
        }
        else if( "real" == p_words[2] )
        {
            int sub_seed = SeedsBase::GetInstance()->GetSeed( StringToInt( p_words[5] ) );

            std::uniform_real_distribution<dsreal>* source = _DRAWSPACE_NEW_( std::uniform_real_distribution<dsreal>, std::uniform_real_distribution<dsreal>( StringToReal( p_words[3] ), StringToReal( p_words[4] ) ) );
            UniformRealRandom* random = _DRAWSPACE_NEW_( UniformRealRandom, UniformRealRandom( source, sub_seed ) );
            
            random_source = random;
        }
    }

    notify_parent( p_stack, random_source );

    return true;
}

void RandomDistributionParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount )
{
}


bool RepeatParser::Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack )
{
    Repeat* rep = _DRAWSPACE_NEW_( Repeat, Repeat );
    
    notify_parent( p_stack, rep );

    StackEntry se;
    se.arg_counter = 0;
    se.parser = this;
    se.atomic = rep;
    p_stack.push( se );

    return true;
}

void RepeatParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount )
{
    // get associated repeater
    Repeat* rep = static_cast<Repeat*>( p_parent ); 

    switch( p_argcount )
    {
        case 0:

            rep->SetNbLoops( p_child );
            break;

        case 1:

            rep->SetChild( p_child );
            break;
    }
}


bool BatchParser::Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack )
{
    Batch* batch = _DRAWSPACE_NEW_( Batch, Batch );
    
    notify_parent( p_stack, batch );

    StackEntry se;
    se.arg_counter = 0;
    se.parser = this;
    se.atomic = batch;
    p_stack.push( se );

    return true;
}

void BatchParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount )
{
    // get associated batch
    Batch* batch = static_cast<Batch*>( p_parent ); 
    batch->AddChild( p_child );
}

bool IndexParser::Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack )
{
    Index* index = _DRAWSPACE_NEW_( Index, Index );
    
    notify_parent( p_stack, index );

    StackEntry se;
    se.arg_counter = 0;
    se.parser = this;
    se.atomic = index;
    p_stack.push( se );

    return true;
}

void IndexParser::SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount )
{
    // get associated index
    Index* index = static_cast<Index*>( p_parent ); 

    switch( p_argcount )
    {
        case 0:

            index->SetIndex( p_child );
            break;

        case 1:
            {
                Array* array = dynamic_cast<Array*>( p_child );
                if( !array )
                {
                    _DSEXCEPTION( "Procedural rules parser : child 2 of an index must be an array" );
                }
                index->SetArray( array );
            }            
            break;
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RulesPackage::RulesPackage( DrawSpace::Core::BaseCallback<void, Atomic*>* p_handler )
{
    m_root = _DRAWSPACE_NEW_( RootParser, RootParser );
        
    StackEntry se;
    se.arg_counter = 0;
    se.parser = m_root;
    se.atomic = NULL;
    m_stack.push( se );

    m_opcodes["pub"] = _DRAWSPACE_NEW_( PubParser, PubParser( p_handler ) );
    m_opcodes["integer"] = _DRAWSPACE_NEW_( IntegerParser, IntegerParser );
    m_opcodes["real"] = _DRAWSPACE_NEW_( RealParser, RealParser );
    m_opcodes["string"] = _DRAWSPACE_NEW_( StringParser, StringParser );
    m_opcodes["vector"] = _DRAWSPACE_NEW_( VectorParser, VectorParser );
    m_opcodes["array"] = _DRAWSPACE_NEW_( ArrayParser, ArrayParser );
    m_opcodes["random"] = _DRAWSPACE_NEW_( RandomDistributionParser, RandomDistributionParser );
    m_opcodes["repeat"] = _DRAWSPACE_NEW_( RepeatParser, RepeatParser );
    m_opcodes["batch"] = _DRAWSPACE_NEW_( BatchParser, BatchParser );
    m_opcodes["index"] = _DRAWSPACE_NEW_( IndexParser, IndexParser );
}
    
RulesPackage::~RulesPackage( void )
{
}

bool RulesPackage::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "end" == p_words[0] )
    {
        m_stack.pop();
    }
    else if( "#" == p_words[0] )
    {
        // do nothin'
        return true;
    }
    else
    {
        if( m_opcodes.count( p_words[0] ) > 0 )
        {            
            if( false == m_opcodes[p_words[0]]->Parse( p_line_num, p_words, m_stack ) )
            {
                _PARSER_MISSING_ARG__
                _DSEXCEPTION( m_lasterror );
                return false;
            }    
        }
        else
        {            
            _PARSER_UNEXPECTED_KEYWORD_
            _DSEXCEPTION( m_lasterror );
            return false;
        }
    }
    return true;
}

RootParser* RulesPackage::GetRootParser( void )
{
    return m_root;
}