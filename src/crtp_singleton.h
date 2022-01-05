/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

// Curiously recurring template pattern for singleton :-D

#ifndef _CRTP_SINGLETON_H_
#define _CRTP_SINGLETON_H_

namespace DrawSpace
{
namespace Utils
{

template<class T>
class BaseSingleton
{
protected:
    static T* m_instance;

public:
    static T* GetInstance(void)
    {
        if (!m_instance)
        {
            //static T t;
            //m_instance = &t;
            m_instance = new T;
        }
        return m_instance;
    };
};
}
}

template <class T>
T* DrawSpace::Utils::BaseSingleton<T>::m_instance;

#endif
