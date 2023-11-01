
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

namespace renderMe
{
	namespace core
	{
		template<typename T=double, int Size=4>
		class Vector
        {
        public:

            Vector(void) = default;
            ~Vector(void) = default;

            /*
            dsreal operator[](int p_index) const
            {
                return m_vector[p_index];
            };

            dsreal& operator[](int p_index)
            {
                return m_vector[p_index];
            };

            dsreal lengthPow2(void) const;
            dsreal length(void) const;

            void normalize(void);
            void scale(dsreal p_scale);

            static Vector lerp(const Vector& p_v1, const Vector& p_v2, dsreal p_blend);

            static Vector prodVec(const DrawSpace::Maths::Vector& p_vA, const DrawSpace::Maths::Vector& p_vB);
            */

        private:
            T m_vector[Size] = { 0 };
        };
	}
}
