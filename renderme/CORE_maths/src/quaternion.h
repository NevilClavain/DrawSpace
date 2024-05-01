
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

#include "tvector.h"
#include "matrix.h"

namespace renderMe
{
	namespace core
	{
		namespace maths
		{
			class Quaternion
			{
			public:
				Quaternion(void);
				Quaternion(double p_x, double p_y, double p_z, double p_w);

				~Quaternion(void) = default;

				double operator[](int p_index) const
				{
					return m_quat[p_index];
				};

				double& operator[](int p_index)
				{
					return m_quat[p_index];
				};

				void zero(void);
				void identity(void);
				void rotationAxis(const Real3Vector& p_axis, double p_angle);
				void rotationMatFrom(Matrix& p_mat) const;
				void normalize(void);

				static Quaternion lerp(const Quaternion& p_q1, const Quaternion& p_q2, double p_blend);

				// p_forward : vector from destination to source !
				static Quaternion lookRotation(const Real3Vector& p_forward, const Real3Vector& p_up);

			private:
				double      m_quat[4] = { 0.0 };
			};

			Quaternion operator* (const Quaternion& p_qA, const Quaternion& p_qB);
		}
	}
}

//renderMe::core::maths::Quaternion operator* (const renderMe::core::maths::Quaternion& p_qA, const renderMe::core::maths::Quaternion& p_qB);