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

#ifndef _MODULE_KEYS_H_
#define _MODULE_KEYS_H_

#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Module
{

class KeySinkBase
{
protected:
    dsstring     m_id;

public:

    KeySinkBase( const dsstring& p_id ) : m_id( p_id )
    {
    }

    dsstring GetId( void )
    {
        return m_id;
    }

};


template <typename base>
class KeySink : public KeySinkBase
{
protected:

public:

    KeySink( const dsstring& p_id ) : KeySinkBase( p_id )
    {
    }

    virtual void OnUpdated( base p_val )
    {
    }
};





class KeySourceBase
{
protected:

        dsstring     m_id;

public:

    KeySourceBase( const dsstring& p_id ) : m_id( p_id )
    {
    }

    dsstring GetId( void )
    {
        return m_id;
    }


    virtual void AddSink( KeySinkBase* p_sink ) = 0;

};



template <typename base>
class KeySource : public KeySourceBase
{
protected:

    std::vector<KeySink<base>*>     m_sinks;
    std::set<KeySinkBase*>          m_sinkskey;

public:

    KeySource( const dsstring& p_id )  : KeySourceBase( p_id )
    {
    };

    void AddSink( KeySinkBase* p_sink )
    {
        if( m_sinkskey.count( p_sink ) == 0 )
        {
            // si ce ptr n'est pas deja enregistre...
            m_sinks.push_back( static_cast<KeySink<base>*>( p_sink ) );
            m_sinkskey.insert( p_sink );
        }
    }


    void operator= ( base p_val )
    {
        for( size_t i = 0; i < m_sinks.size(); i++ )
        {
            m_sinks[i]->OnUpdated( p_val );
        }
    }
};


class KeysLinkTable
{
protected:

    std::vector<KeySourceBase*> m_clientsKeys;
    std::vector<KeySinkBase*>   m_moduleKeys;

    void updateLinks( void );


public:

    KeysLinkTable( void );
    ~KeysLinkTable( void );

    void RegisterClientKey( KeySourceBase* p_key );
    void RegisterModuleKey( KeySinkBase* p_key );
    void RegisterModuleKeysVector( const std::vector<DrawSpace::Module::KeySinkBase*>& p_keys );
};

}
}

#endif
