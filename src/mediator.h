/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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

#ifndef _MEDIATOR_H_
#define _MEDIATOR_H_

#ifdef WIN32
#include <typeinfo.h>
#else
#include <typeinfo>
#endif

#include <list>
#include "drawspace_commons.h"
#include "memalloc.h"
#include "mutex.h"

namespace DrawSpace
{
namespace Threading
{

class IProperty
{
public:
    virtual ~IProperty( void ) { };

    virtual void    GetName( std::string& p_name ) = 0;
    virtual void    GetTypeId( std::string& p_typeid ) = 0;

    virtual IProperty*   CloneMe( void ) = 0;
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

    virtual IProperty*  CloneMe( void ) 
                        {
                            TypedProperty<base>* elt;
                            elt = new TypedProperty<base>( m_name, m_value );
                            return elt;
                        };
};


class PropertyPool
{
private:

    //DrawSpace::Utils::Mutex m_mutex;
    std::vector<IProperty*> m_props;

public:

    PropertyPool( void ) { };

    // copy constructor
    PropertyPool( const PropertyPool& p_other )
    {
        for( size_t i = 0; i < p_other.m_props.size(); i++ )
        {
            m_props.push_back( p_other.m_props[i]->CloneMe() );
        }
    }
    

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
        //m_mutex.WaitInfinite();
        size_t nbp = m_props.size();
        //m_mutex.Release();

        return nbp;
    };

    void Clear( void )
    {
        //m_mutex.WaitInfinite();
        m_props.clear();
        //m_mutex.Release();
    };

    template<typename base>
    void AddPropValue( const char* p_name, base p_propvalue )
    {
        //m_mutex.WaitInfinite();

        TypedProperty<base>* prop = new TypedProperty<base>( p_name, p_propvalue );
        m_props.push_back( prop );

        //m_mutex.Release();
    };

    // when specifying a name is annoying and useless...
    template<typename base>
    void AddPropValue( base p_propvalue )
    {
        TypedProperty<base>* prop = new TypedProperty<base>( "", p_propvalue );
        m_props.push_back( prop );
    };


    template<typename base>
    void AddProp( const char* p_name )
    {
        //m_mutex.WaitInfinite();

        TypedProperty<base>* prop = new TypedProperty<base>( p_name );
        m_props.push_back( prop );

        //m_mutex.Release();
    };

    // when specifying a name is annoying and useless...
    template<typename base>
    void AddProp( void )
    {
        //m_mutex.WaitInfinite();

        TypedProperty<base>* prop = new TypedProperty<base>( "" );
        m_props.push_back( prop );

        //m_mutex.Release();
    };


    // when specifying a name is annoying and useless...
    template<typename base>
    base GetPropValue( void )
    {
        base dummy;
        std::vector<IProperty*>::iterator it;

        //m_mutex.WaitInfinite();
        for( it = m_props.begin(); it != m_props.end(); ++it )
        {
            TypedProperty<base>* prop = dynamic_cast<TypedProperty<base>*>( (*it) );

            if( prop )
            {
                dummy = prop->m_value;
                break;
            }            
        }
        //m_mutex.Release();
        return dummy;
    };



    template<typename base>
    base GetPropValue( const char* p_name )
    {
        base dummy;
        std::vector<IProperty*>::iterator it;

        //m_mutex.WaitInfinite();
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
        //m_mutex.Release();
        return dummy;
    };

    // when specifying a name is annoying and useless...
    template<typename base>
    void SetPropValue( base p_propvalue )
    {
        std::vector<IProperty*>::iterator it;

        
        for( it = m_props.begin(); it != m_props.end(); ++it )
        {
            TypedProperty<base>* prop = dynamic_cast<TypedProperty<base>*>( (*it) );

            if( prop )
            {
                prop->m_value = p_propvalue;
                break;
            }            
        }        
    };


    template<typename base>
    void SetPropValue( const char* p_name, base p_propvalue )
    {
        std::vector<IProperty*>::iterator it;

        //m_mutex.WaitInfinite();
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
        //m_mutex.Release();
    };

    PropertyPool& operator=( const PropertyPool& p_other ) 
    {
        for( size_t i = 0; i < p_other.m_props.size(); i++ )
        {
            m_props.push_back( p_other.m_props[i]->CloneMe() );
        }
        return *this;
    };

    void GetPropertiesNamesList( std::vector<dsstring>& p_outlist )
    {
        for( size_t i = 0; i < m_props.size(); i++ )
        {
            dsstring pname;
            m_props[i]->GetName( pname );
            p_outlist.push_back( pname );
        }
    }
   
};

class Mediator
{
public:
    class MessageQueue
    {

    protected:
        HANDLE                      m_system_event;
        DrawSpace::Utils::Mutex     m_mutex;
        std::list<PropertyPool>     m_queue;

    public:

        MessageQueue( HANDLE p_handle ) :        
        m_system_event( p_handle )
        {
        }

        HANDLE GetHandle( void )
        {
            return m_system_event;
        }

        void PushMessage( const PropertyPool& p_msg )
        {
            m_mutex.WaitInfinite();

            m_queue.push_front( p_msg );
            SetEvent( m_system_event );

            m_mutex.Release();
        }

        bool GetNextMessage( PropertyPool& p_msg )
        {
            bool next = false;

            if( m_queue.size() > 0 )
            {
                m_mutex.WaitInfinite();

                p_msg = m_queue.back();
                m_queue.pop_back();

                m_mutex.Release();

                next = true;
            }
            else
            {
                next = false;
            }

            return next;
        }

        friend class Mediator;
    };

protected:
    static Mediator* m_instance;

    int                                     m_nb_handles;
    HANDLE                                  m_handles[512];    
    std::map<HANDLE, MessageQueue*>         m_messages_by_handle;
    
public:

    Mediator( void );
    ~Mediator( void );

    MessageQueue* CreateMessageQueue( void );
    MessageQueue* Wait( void );
};
}
}

#endif
