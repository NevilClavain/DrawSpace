/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#ifndef _MEDIATOR_H_
#define _MEDIATOR_H_

#ifdef WIN32
#include <typeinfo.h>
#else
#include <typeinfo>
#endif

#include "drawspace_commons.h"
#include "memalloc.h"
#include "mutex.h"

namespace DrawSpace
{
namespace Core
{

class IProperty
{
public:
    virtual ~IProperty( void ) { };

    virtual void    GetName( std::string& p_name ) = 0;
    virtual void    GetTypeId( std::string& p_typeid ) = 0;
};

template<typename base>
class TypedProperty : public IProperty
{
protected:
    dsstring		m_name;

public:
    base            m_value;

    TypedProperty( const dsstring &p_name ) : m_name( p_name ) { };
    TypedProperty( const dsstring &p_name, base p_initval ) : m_name( p_name ), m_value( p_initval ) { };
    virtual void            GetName( dsstring& p_name ) { p_name = m_name; };
    virtual void            GetTypeId( dsstring& p_typeid ) { p_typeid = typeid( base ).name(); };
};


class PropertyPool
{
private:

    DrawSpace::Utils::Mutex m_mutex;

    std::vector<IProperty*> m_props;
public:
    ~PropertyPool( void )
    {
        std::vector<IProperty*>::iterator it;

        for( it = m_props.begin(); it != m_props.end(); ++it )
        {
            delete (*it);
        }
    }

    size_t Size( void )
    {
        m_mutex.WaitInfinite();
        size_t nbp = m_props.size();
        m_mutex.Release();

        return nbp;
    };

    void Clear( void )
    {
        m_mutex.WaitInfinite();
        m_props.clear();
        m_mutex.Release();
    };

    template<typename base>
    void AddPropValue( const char* p_name, base p_propvalue )
    {
        m_mutex.WaitInfinite();

        TypedProperty<base>* prop = new TypedProperty<base>( p_name, p_propvalue );
        m_props.push_back( prop );

        m_mutex.Release();
    };

    template<typename base>
    void AddProp( const char* p_name )
    {
        m_mutex.WaitInfinite();

        TypedProperty<base>* prop = new TypedProperty<base>( p_name );
        m_props.push_back( prop );

        m_mutex.Release();
    };


    template<typename base>
    base GetPropValue( const char* p_name )
    {
        base dummy;
        std::vector<IProperty*>::iterator it;

        m_mutex.WaitInfinite();
        for( it = m_props.begin(); it != m_props.end(); ++it )
        {
            TypedProperty<base>* prop = dynamic_cast<TypedProperty<base>*>( (*it) );

            if( prop )
            {
                std::string name;
                prop->GetName( name );

                if( name == p_name )
                {
                    dummy = prop->m_value;
                    break;
                }
            }            
        }
        m_mutex.Release();
        return dummy;
    };


    template<typename base>
    void SetPropValue( const char* p_name, base p_propvalue )
    {
        std::vector<IModuleProperty*>::iterator it;

        m_mutex.WaitInfinite();
        for( it = m_props.begin(); it != m_props.end(); ++it )
        {
            TypedProperty<base>* prop = dynamic_cast<TypedProperty<base>*>( (*it) );

            if( prop )
            {
                dsstring name;
                prop->GetName( name );

                if( name == p_name )
                {
                    prop->m_value = p_propvalue;
                    break;
                }
            }            
        }
        m_mutex.Release();
    };
};

class Mediator
{
protected:
    static Mediator* m_instance;

    typedef struct
    {
        dsstring                    name;
        HANDLE                      system_event;        
        PropertyPool*               args;        

    } Event;

    std::map<dsstring, Event>       m_events_by_name;
    std::map<HANDLE, Event>         m_events_by_handle;

    int                             m_nb_handles;
    HANDLE                          m_handles[512];
    

    Mediator( void );
public:

    ~Mediator( void );

    static Mediator* GetInstance( void )
    {
        if( NULL == m_instance )
        {
            m_instance = _DRAWSPACE_NEW_( Mediator, Mediator );
        }
        return m_instance;
    }

    bool CreateEvent( const dsstring& p_eventname );

    void Notify( const dsstring& p_eventname );
    bool Wait( dsstring& p_eventname );
    PropertyPool* GetEventPropertyPool( const dsstring& p_eventname );

};
}
}

#endif