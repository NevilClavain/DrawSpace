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

template <typename ReturnType, typename Parameter, typename Parameter2>
class BaseCallback2
{
public:
    virtual ReturnType operator()( Parameter p_parameter, Parameter2 p_parameter2 ) = 0;
    virtual ReturnType Execute( Parameter p_parameter, Parameter2 p_parameter2 ) = 0;
};


template <typename ReturnType, typename Parameter, typename Parameter2, typename Parameter3>
class BaseCallback3
{
public:
    virtual ReturnType operator()( Parameter p_parameter, Parameter2 p_parameter2, Parameter3 p_parameter3 ) = 0;
    virtual ReturnType Execute( Parameter p_parameter, Parameter2 p_parameter2, Parameter3 p_parameter3 ) = 0;
};


template <typename ReturnType, typename Parameter, typename Parameter2, typename Parameter3, typename Parameter4>
class BaseCallback4
{
public:
    virtual ReturnType operator()( Parameter p_parameter, Parameter2 p_parameter2, Parameter3 p_parameter3, Parameter4 p_parameter4 ) = 0;
    virtual ReturnType Execute( Parameter p_parameter, Parameter2 p_parameter2, Parameter3 p_parameter3, Parameter4 p_parameter4 ) = 0;
};


template <class Class, typename ReturnType, typename Parameter>
class CallBack : public BaseCallback<ReturnType, Parameter>
{
public:
    typedef ReturnType ( Class::*Method )( Parameter );

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


template <class Class, typename ReturnType, typename Parameter, typename Parameter2>
class CallBack2 : public BaseCallback2<ReturnType, Parameter, Parameter2>
{
public:
    typedef ReturnType ( Class::*Method )( Parameter, Parameter2 );

protected:
    Class* m_class_instance;
    Method m_method;

public:
    CallBack2( Class* p_class_instance, Method p_method ) : m_class_instance( p_class_instance ), m_method( p_method )
    {
    }

    virtual ReturnType operator()( Parameter p_parameter, Parameter2 p_parameter2 )
    {
        return ( m_class_instance->*m_method )( p_parameter, p_parameter2 );
    }

    virtual ReturnType Execute( Parameter p_parameter, Parameter2 p_parameter2 )
    {
        return operator()( p_parameter, p_parameter2 );
    }
};


template <class Class, typename ReturnType, typename Parameter, typename Parameter2, typename Parameter3>
class CallBack3 : public BaseCallback3<ReturnType, Parameter, Parameter2, Parameter3>
{
public:
    typedef ReturnType ( Class::*Method )( Parameter, Parameter2, Parameter3 );

protected:
    Class* m_class_instance;
    Method m_method;

public:
    CallBack3( Class* p_class_instance, Method p_method ) : m_class_instance( p_class_instance ), m_method( p_method )
    {
    }

    virtual ReturnType operator()( Parameter p_parameter, Parameter2 p_parameter2, Parameter3 p_parameter3 )
    {
        return ( m_class_instance->*m_method )( p_parameter, p_parameter2, p_parameter3 );
    }

    virtual ReturnType Execute( Parameter p_parameter, Parameter2 p_parameter2, Parameter3 p_parameter3 )
    {
        return operator()( p_parameter, p_parameter2, p_parameter3 );
    }
};



template <class Class, typename ReturnType, typename Parameter, typename Parameter2, typename Parameter3, typename Parameter4>
class CallBack4 : public BaseCallback4<ReturnType, Parameter, Parameter2, Parameter3, Parameter4>
{
public:
    typedef ReturnType ( Class::*Method )( Parameter, Parameter2, Parameter3, Parameter4 );

protected:
    Class* m_class_instance;
    Method m_method;

public:
    CallBack4( Class* p_class_instance, Method p_method ) : m_class_instance( p_class_instance ), m_method( p_method )
    {
    }

    virtual ReturnType operator()( Parameter p_parameter, Parameter2 p_parameter2, Parameter3 p_parameter3, Parameter4 p_parameter4 )
    {
        return ( m_class_instance->*m_method )( p_parameter, p_parameter2, p_parameter3, p_parameter4 );
    }

    virtual ReturnType Execute( Parameter p_parameter, Parameter2 p_parameter2, Parameter3 p_parameter3, Parameter4 p_parameter4 )
    {
        return operator()( p_parameter, p_parameter2, p_parameter3, p_parameter4 );
    }
};


}
}

#endif
