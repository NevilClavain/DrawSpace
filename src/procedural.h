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

#ifndef _PROCEDURAL_H_
#define _PROCEDURAL_H_

#include "drawspace_commons.h"
#include "vector.h"
#include "callback.h"
#include "memalloc.h"
#include "parser.h"
#include <random>
#include <stack>

namespace DrawSpace
{
namespace Procedural
{
   
class Atomic
{
public:

    Atomic( void ) { }
    virtual ~Atomic( void ) { };

    virtual void Apply( void ) = 0;
    virtual Atomic* GetResultValue( void ) = 0;

};


class Integer : public Atomic
{
protected:
    int     m_value;

public:
    virtual int GetValue( void ) { return m_value; }
    virtual void SetValue( int p_value ) { m_value = p_value; }

    virtual void Apply( void ) {}
    virtual Atomic* GetResultValue( void )
    {
        return this;
    }
};

class Real : public Atomic
{
protected:
    dsreal  m_value;

public:
    virtual dsreal GetValue( void ) { return m_value; }
    virtual void SetValue( dsreal p_value ) { m_value = p_value; }

    virtual void Apply( void ) {}
    virtual Atomic* GetResultValue( void )
    {
        return this;
    }
};

class String: public Atomic
{
protected:
    dsstring  m_value;

public:
    virtual dsstring GetValue( void ) { return m_value; }
    virtual void SetValue( const dsstring &p_value ) { m_value = p_value; }

    virtual void Apply( void ) {}
    virtual Atomic* GetResultValue( void )
    {
        return this;
    }
};

class Vector : public Atomic
{
protected:
    DrawSpace::Utils::Vector    m_value;

public:

    virtual DrawSpace::Utils::Vector GetValue( void ) { return m_value; }
    virtual void SetValue( const DrawSpace::Utils::Vector p_value ) { m_value = p_value; }

    virtual void Apply( void ) {}
    virtual Atomic* GetResultValue( void )
    {
        return this;
    }
};

class Array : public Atomic
{
protected:
    std::vector<Atomic*>        m_value;

public:

    virtual int GetSize( void );
    virtual Atomic* GetValueAt( int p_index );
    virtual void AddValue( Atomic* p_value );

    virtual void Apply( void );

    virtual Atomic* GetResultValue( void )
    {
        return this;
    }
};


class Publisher : public Atomic
{
protected:

    Atomic*                                                 m_child;
    DrawSpace::Core::BaseCallback<void, Atomic*>*           m_handler;

public:

    Publisher( void );

    virtual void SetChild( Atomic* p_child ) { m_child = p_child; }
    virtual Atomic* GetChild( void ) { return m_child; }
    virtual void RegisterHandler( DrawSpace::Core::BaseCallback<void, Atomic*>* p_handler ) { m_handler = p_handler; }

    virtual void Apply( void );
    virtual Atomic* GetResultValue( void );

};

template <typename Base, typename DistributionType, typename AtomType>
class RandomDistribution : public Atomic
{
protected:
    std::default_random_engine              m_generator;
    DistributionType*                       m_distribution;
    AtomType                                m_result;
    
public:
    
    RandomDistribution( DistributionType* p_distribution_type, int p_seed )
    {
        //m_distribution = _DRAWSPACE_NEW_( DistributionType, DistributionType( p_arg1, p_arg2 ) );
        m_distribution = p_distribution_type;
        m_generator.seed( p_seed );  
    }
    virtual ~RandomDistribution( void )
    {
        //_DRAWSPACE_DELETE_( m_distribution );    
    }
    virtual void Apply( void )
    {
        m_result.SetValue( (*m_distribution)( m_generator ) );
    }

    virtual Atomic* GetResultValue( void )
    {
        return &m_result;
    }
};


class Repeat : public Atomic
{
protected:
    Atomic*                                                 m_child;
    Atomic*                                                 m_nbloops;

public:
    Repeat( void );
    virtual ~Repeat( void );
    
    virtual void SetChild( Atomic* p_child ) { m_child = p_child; }
    virtual void SetNbLoops( Atomic* p_nbloops ) { m_nbloops = p_nbloops; }

    virtual void Apply( void );
    virtual Atomic* GetResultValue( void );

};


class Batch : public Atomic
{
protected:

    std::vector<Atomic*>                m_list;

public:
    Batch( void );
    virtual ~Batch( void );

    virtual void AddChild( Atomic* p_child );

    virtual void Apply( void );
    virtual Atomic* GetResultValue( void );

};

class Index : public Atomic
{
protected:

    Array*                  m_array;
    //Integer*                m_index;
    Atomic*                 m_index;

    Atomic*                 m_result;

public:

    Index( void );
    virtual ~Index( void );

    virtual void SetIndex( Atomic* p_index );
    virtual void SetArray( Array* p_array );

    virtual void Apply( void );
    virtual Atomic* GetResultValue( void );

};


class OpcodeParser
{
public:
    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, std::stack<std::pair<OpcodeParser*, Atomic*>>& p_stack ) = 0;
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child ) = 0;
};

class RootParser : public OpcodeParser
{
protected:
    Atomic* m_rules;

public:
    
    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, std::stack<std::pair<OpcodeParser*, Atomic*>>& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child );
    virtual Atomic* GetRules( void );
};

class PubParser : public OpcodeParser
{
protected:    
    DrawSpace::Core::BaseCallback<void, Atomic*>*   m_handler;

public:
    PubParser( DrawSpace::Core::BaseCallback<void, Atomic*>* p_handler );
    virtual ~PubParser( void );

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, std::stack<std::pair<OpcodeParser*, Atomic*>>& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child );

};

class IntegerParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, std::stack<std::pair<OpcodeParser*, Atomic*>>& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child );
};


class RealParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, std::stack<std::pair<OpcodeParser*, Atomic*>>& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child );
};

class StringParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, std::stack<std::pair<OpcodeParser*, Atomic*>>& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child );
};

class VectorParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, std::stack<std::pair<OpcodeParser*, Atomic*>>& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child );
};

class ArrayParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, std::stack<std::pair<OpcodeParser*, Atomic*>>& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child );
};

typedef Procedural::RandomDistribution<int, std::uniform_int_distribution<int>, Procedural::Integer> UniformIntegerRandom;

class RandomDistributionParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, std::stack<std::pair<OpcodeParser*, Atomic*>>& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child );
};


class RulesPackage : public Utils::Parser
{
protected:
    
    RootParser*                                     m_root;
    std::map<dsstring, OpcodeParser*>               m_opcodes;    
    std::stack<std::pair<OpcodeParser*, Atomic*>>   m_stack;

    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:

    RulesPackage( DrawSpace::Core::BaseCallback<void, Atomic*>* p_handler );
    virtual ~RulesPackage( void );

    RootParser* GetRootParser( void );

};

////////////////////////////////////////////////////////////////////////////////////////

}
}

#endif