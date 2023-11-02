
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

#pragma once

#include <cmath>
#include <string>
#include <tuple>

namespace renderMe
{
	namespace core
	{
		template<typename T=double, int Size=4>
		class Vector
        {
        public:            
            Vector() = default;

            template<class... Args>
            Vector(Args... p_args)
            {
                unpack_args(0, p_args...);
            }

            ~Vector() = default;

            std::string dump() const
            {
                std::string content{ "[ " };
                for(int i = 0; i < Size; i++)
                { 
                    content += std::to_string(m_vector[i]) + " ";
                }

                content += "]";
                return content;
            }

            T operator[](size_t p_index) const
            {
                return m_vector[p_index];
            };

            T& operator[](size_t p_index)
            {
                return m_vector[p_index];
            };

            T lengthPow2(void) const
            {
                T sum{ 0 };
                for (int i = 0; i < Size; i++)
                {
                    sum += m_vector[i] * m_vector[i];
                }
                return sum;
            }

            T length(void) const
            {
                return std::sqrt(lengthPow2());
            }

            void normalize(void)
            {
                const T len{ length() };
                if (len > 0)
                {
                    const T mag{ static_cast<T>(1) / len };

                    for (int i = 0; i < Size; i++)
                    {
                        m_vector[i] *= mag;
                    }
                }
            }

        private:                   
            T m_vector[Size] = { 0 };

            // to stop the recursive args unpacking from the variadic
            void unpack_args(int index) {}

            template <class First, class... Rest>
            void unpack_args(int index, First first, Rest... rest) {

                m_vector[index] = first;
                unpack_args(index + 1, rest...); // Unpack the arguments for further treatment
            }
        };
	}
}
