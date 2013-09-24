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

#ifndef _CALLBACK_H_
#define _CALLBACK_H_

namespace DrawSpace
{
namespace Core
{
template <typename ReturnType, typename Parameter>
class BaseCallback
{
public:
    virtual ReturnType operator()( Parameter p_parameter ) = 0;
    virtual ReturnType Execute( Parameter p_parameter ) = 0;
};

template <class Class, typename ReturnType, typename Parameter>
class CallBack : public BaseCallback<ReturnType, Parameter>
{
public:
    typedef ReturnType (Class::*Method)(Parameter);

protected:
    Class* m_class_instance;
    Method m_method;

public:
    CallBack( Class* p_class_instance, Method p_method ) : m_class_instance( p_class_instance ), m_method( p_method )
    {
    }

    virtual ReturnType operator()( Parameter p_parameter )
    {
      return ( m_class_instance->*m_method )( p_parameter );
    }

    virtual ReturnType Execute( Parameter p_parameter )
    {
      return operator()( p_parameter );
    }
};
}
}

#endif