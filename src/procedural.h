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
    Integer*                m_index;

    Atomic*                 m_result;

public:

    Index( void );
    virtual ~Index( void );

    virtual void SetIndex( Integer* p_index );
    virtual void SetArray( Array* p_array );

    virtual void Apply( void );
    virtual Atomic* GetResultValue( void );

};

////////////////////////////////////////////////////////////////////////////////////////

}
}

#endif