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

#include "drawspace_commons.h"
#include "memalloc.h"

#ifndef _ENTITY_H_
#define _ENTITY_H_

namespace DrawSpace
{
namespace Core
{

class BaseComponent abstract {};

template<typename T>
using ComponentPurpose = std::unique_ptr<T>;

template<typename T>
class Component sealed : public BaseComponent
{
private:
    ComponentPurpose<T>       m_purpose;

public:

    template<class... Args>
    void MakePurpose(Args&&... p_args)
    {
        m_purpose = std::make_unique<T>((std::forward<Args>(p_args))...);
    }

    virtual T& getPurpose(void) const
    {
        return *( m_purpose.get() );
    }
};

class Aspect abstract
{
protected:
    std::map<dsstring, BaseComponent*> m_components;

public:

    Aspect( void ) {};
    virtual ~Aspect( void ) {}
   
    template<typename T, class... Args>
    void AddComponent( const dsstring& p_id, Args&&... p_args )
    {
        if( m_components.count( p_id ) > 0 )
        {
            _DSEXCEPTION( "Component with same id already exists : " + p_id );
        }

        Component<T>* newcomp =  _DRAWSPACE_NEW_( Component<T>, Component<T> );
        newcomp->MakePurpose( (std::forward<Args>(p_args))... );
        m_components[p_id] = newcomp;
    }
        
    template<typename T>
    void RemoveComponent( const dsstring& p_id )
    {
        if( 0 == m_components.count( p_id ) )
        {
            _DSEXCEPTION( "Component id not registered in this aspect : " + p_id );
        }

        Component<T>* comp = static_cast<Component<T>*>( m_components[p_id] );

        _DRAWSPACE_DELETE_( comp );
        m_components.erase( p_id );
    }
    
    template<typename T>
    Component<T>* GetComponent( const dsstring& p_id )
    {
        if( 0 == m_components.count( p_id ) )
        {
            _DSEXCEPTION( "Component id not registered in this aspect : " + p_id );
        }

        Component<T>* comp = static_cast<Component<T>*>( m_components[p_id] );
        
        return comp;
    }
};

class Entity sealed
{
private:
    std::unordered_map<size_t, Aspect*> m_aspects;

public:

    Entity( void ) {};
    ~Entity( void )
    {
        for( auto it = m_aspects.begin(); it != m_aspects.end(); ++it )
        {
            _DRAWSPACE_DELETE_( it->second );
        }
    }
    

    template<typename T>
    void AddAspect( void )
    {
        if( m_aspects.count(typeid(T).hash_code() ) )
        {
            _DSEXCEPTION( "Aspect type already exists in this entity : " + dsstring( typeid(T).name() ) );
        }
        m_aspects[typeid(T).hash_code()] = _DRAWSPACE_NEW_( T, T );
    }

    template<typename T>
    T* GetAspect( void )
    {
        size_t tid = typeid(T).hash_code();

        if( 0 == m_aspects.count( tid ) )
        {
            _DSEXCEPTION( "Aspect type not registered in this entity : " + dsstring( typeid(T).name() ) );
        }
        
        T* aspect = static_cast<T*>( m_aspects[tid] );
        return aspect;
    }
};
}
}


#endif