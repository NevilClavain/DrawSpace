
/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */


// Curiously recurring template pattern for singleton :-D

#pragma once
#include <memory>

namespace renderMe
{
    namespace core
    {
        template<class T>
        class Singleton
        {
        public:
            ~Singleton() = default;

            static T* getInstance(void)
            {
                if (!m_instance)
                {
                    m_instance = std::make_unique<T>();
                }
                return m_instance.get();
            };

        protected:
            Singleton() = default;
            static std::unique_ptr<T> m_instance;
        };
    }
}

template <class T>
std::unique_ptr<T> renderMe::core::Singleton<T>::m_instance;
