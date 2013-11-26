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

#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#ifdef WIN32
#include <typeinfo.h>
#else
#include <typeinfo>
#endif

#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Core
{
class Property
{
public:
    virtual ~Property( void ) { };

    virtual void    GetName( std::string& p_name ) = 0;
    virtual void    GetTypeId( std::string& p_typeid ) = 0;
};

template<typename base>
class TypedProperty : public Property
{
protected:
    std::string		m_name;

public:
    base            m_value;

    TypedProperty( const dsstring& p_name ) : m_name( p_name ) { };
    TypedProperty( const dsstring& p_name, base p_initval ) : m_name( p_name ), m_value( p_initval ) { };
    virtual ~TypedProperty( void ) { };

    virtual void            GetName( std::string& p_name ) { p_name = m_name; };
    virtual void            GetTypeId( std::string& p_typeid ) { p_typeid = typeid( base ).name(); };
};
}
}

#endif