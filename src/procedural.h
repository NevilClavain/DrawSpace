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
#include <random>

namespace DrawSpace
{
namespace Procedural
{
   
class Atomic
{
protected:
    dsstring m_id;

public:

    Atomic( void ) { }
    virtual ~Atomic( void ) { };

    virtual void SetId( const dsstring& p_id );
    virtual void GetId( dsstring& p_id );

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

class Vector : public Atomic
{
protected:
    DrawSpace::Utils::Vector    m_value;

public:
    virtual void GetValue( DrawSpace::Utils::Vector& p_vect ) { p_vect = m_value; }
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
    virtual void GetValue( std::vector<Atomic*>& p_vec ) { p_vec = m_value; }
    virtual void SetValue( const std::vector<Atomic*> p_value ) { m_value = p_value; }

    virtual void Apply( void ) {}
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
    virtual void SetHandler( DrawSpace::Core::BaseCallback<void, Atomic*>* p_handler ) { m_handler = p_handler; }

    virtual void Apply( void );
    virtual Atomic* GetResultValue( void );

};

class UniformDistributionRandom : public Atomic
{
protected:
    std::default_random_engine*             m_generator;
    std::uniform_int_distribution<int>*     m_distribution;
    DrawSpace::Procedural::Integer          m_integer;
    
public:
    
    UniformDistributionRandom( int p_seed, int p_min, int p_max );
    virtual ~UniformDistributionRandom( void );
    virtual void Apply( void );
    virtual Atomic* GetResultValue( void );
};

////////////////////////////////////////////////////////////////////////////////////////

class Source
{
protected:

    Atomic*                                                 m_rules;
    DrawSpace::Core::BaseCallback<void, Atomic*>*           m_handler;

    void set_rules_handler( Atomic* p_atom );

public:

    Source( void );
    ~Source( void );

    void SetRules( Atomic* p_rules );
    void RegisterHandler( DrawSpace::Core::BaseCallback<void, Atomic*>* p_handler );
    void Run( void );

};

}
}

#endif