/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

#ifndef _PROCEDURAL_PARSER_H_
#define _PROCEDURAL_PARSER_H_

#include "procedural_blocs.h"
#include "procedural_seeds.h"

namespace DrawSpace
{
namespace Procedural
{

class OpcodeParser;

typedef struct
{
    OpcodeParser* parser;
    Atomic*       atomic;
    int           arg_counter;

} StackEntry;

typedef std::stack<StackEntry> Stack;


class OpcodeParser
{
protected:

    virtual void notify_parent( Stack& p_stack, Atomic* p_atomic );

public:
    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack ) = 0;
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount ) = 0;
};

class RootParser : public OpcodeParser
{
protected:
    Atomic* m_rules;

public:
    
    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount );
    virtual Atomic* GetRules( void );
};

class PubParser : public OpcodeParser
{
protected:    
    DrawSpace::Core::BaseCallback<void, Atomic*>*   m_handler;

public:
    PubParser( DrawSpace::Core::BaseCallback<void, Atomic*>* p_handler );
    virtual ~PubParser( void );

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount );

};

class IntegerParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount );
};


class RealParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount );
};

class StringParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount );
};

class VectorParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount );
};

class ArrayParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount );
};

typedef Procedural::RandomDistribution<int, std::uniform_int_distribution<int>, Procedural::Integer> UniformIntegerRandom;
typedef Procedural::RandomDistribution<int, std::uniform_real_distribution<dsreal>, Procedural::Real> UniformRealRandom;

class RandomDistributionParser : public OpcodeParser
{
protected:

    DrawSpace::Procedural::SeedsBase* m_seedsbase;

public:

    RandomDistributionParser( DrawSpace::Procedural::SeedsBase* p_seedsbase );
    virtual ~RandomDistributionParser( void );

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount );
};



class RepeatParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount );
};


class BatchParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount );
};

class IndexParser : public OpcodeParser
{
public:

    virtual bool Parse( long p_line_num, std::vector<dsstring>& p_words, Stack& p_stack );
    virtual void SubmitAtomic( Atomic* p_parent, Atomic* p_child, int p_argcount );
};


class RulesPackage : public Utils::Parser
{
protected:
    
    RootParser*                                     m_root;
    std::map<dsstring, OpcodeParser*>               m_opcodes;    
    Stack                                           m_stack;
    DrawSpace::Procedural::SeedsBase                m_seedsbase;

    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:

    RulesPackage( DrawSpace::Core::BaseCallback<void, Atomic*>* p_handler );
    virtual ~RulesPackage( void );

    RootParser* GetRootParser( void );
    void InitializeSeedBase( unsigned int p_seed );
    void InitializeSeedBaseFromCurrentTime( void );

};

}
}

#endif
