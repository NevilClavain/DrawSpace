
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
        namespace maths
        {
            template<typename T = double, int Size = 4>
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
                    for (int i = 0; i < Size; i++)
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


                // access helpers
                T x() const { return m_vector[0]; };
                T y() const { return m_vector[1]; };
                T z() const { return m_vector[2]; };
                T w() const { return m_vector[3]; };

                T r() const { return m_vector[0]; };
                T g() const { return m_vector[1]; };
                T b() const { return m_vector[2]; };
                T a() const { return m_vector[3]; };

                T& x() { return m_vector[0]; };
                T& y() { return m_vector[1]; };
                T& z() { return m_vector[2]; };
                T& w() { return m_vector[3]; };

                T& r() { return m_vector[0]; };
                T& g() { return m_vector[1]; };
                T& b() { return m_vector[2]; };
                T& a() { return m_vector[3]; };

                //////////////////////////////////////////////////////////////////

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

                void scale(T p_scale)
                {
                    for (int i = 0; i < Size; i++)
                    {
                        m_vector[i] *= p_scale;
                    }

                }

                static Vector<T, Size> lerp(const Vector<T, Size>& p_v1, const Vector<T, Size>& p_v2, float p_blend)
                {
                    Vector<T, Size> out;
                    for (int i = 0; i < Size; i++)
                    {
                        out[i] = p_v1[i] + static_cast<T>(p_blend) * (p_v2[i] - p_v1[i]);
                    }
                    return out;
                }

                static Vector<T, Size> crossProduct(const Vector<T, Size>& p_v1, const Vector<T, Size>& p_v2)
                {
                    Vector<T, Size> out;
                    static_assert(Size >= 3, "cross Product applies only on 3D or 4D vector");

                    out[0] = (p_v1[1] * p_v2[2]) - (p_v1[2] * p_v2[1]);
                    out[1] = (p_v1[2] * p_v2[0]) - (p_v1[0] * p_v2[2]);
                    out[2] = (p_v1[0] * p_v2[1]) - (p_v1[1] * p_v2[0]);
                    return out;
                }

                static T dotProduct(const Vector<T, Size>& p_v1, const Vector<T, Size>& p_v2)
                {
                    T sum{ 0 };
                    for (int i = 0; i < Size; i++)
                    {
                        sum += p_v1[i] * p_v2[i];
                    }
                    return sum;
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

            // dot product
            template<typename T = double, int Size = 4>
            T operator* (const Vector<T, Size>& p_vA, const Vector<T, Size>& p_vB)
            {
                return Vector<T, Size>::dotProduct(p_vA, p_vB);
            }

            // sum vectors
            template<typename T = double, int Size = 4>
            Vector<T, Size> operator+ (const Vector<T, Size>& p_vA, const Vector<T, Size>& p_vB)
            {
                Vector<T, Size> sum;
                for (int i = 0; i < Size; i++)
                {
                    sum[i] = p_vA[i] + p_vB[i];
                }

                return sum;
            }


            //Aliases
            using RGBAColor = Vector<unsigned char, 4>;
            using RGBColor = Vector<unsigned char, 3>;
            using IntCoords2D = Vector<int, 2>;
            using Real4Vector = Vector<>;
            using Real3Vector = Vector<double, 3>;

            // Constants
            
            const static Real3Vector ZeroVector  = { 0.0, 0.0, 0.0 };
            const static Real3Vector OneVector   = { 1.0, 1.0, 1.0 };
            const static Real3Vector XAxisVector = { 1.0, 0.0, 0.0 };
            const static Real3Vector YAxisVector = { 0.0, 1.0, 0.0 };
            const static Real3Vector ZAxisVector = { 0.0, 1.0, 0.0 };
            
        }
	} // core
} // renderMe
