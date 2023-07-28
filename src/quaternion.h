/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#pragma once

#include "matrix.h"
#include "vector.h"
#include "maths.h"

namespace DrawSpace
{
	namespace Maths
	{
		class Quaternion
		{
		public:
			Quaternion(void);
			Quaternion(dsreal p_x, dsreal p_y, dsreal p_z, dsreal p_w);

			~Quaternion(void) = default;

			dsreal operator[]( int p_index ) const
			{
				return m_quat[p_index];
			};

			dsreal& operator[]( int p_index )
			{
				return m_quat[p_index];
			};

			void zero( void )
			{
				m_quat[0] = m_quat[1] = m_quat[2] = m_quat[3] = 0.0;
			};

			void identity( void )
			{
				m_quat[0] = 0.0;
				m_quat[1] = 0.0;
				m_quat[2] = 0.0;
				m_quat[3] = 1.0;
			};

			// https://stackoverflow.com/questions/12435671/quaternion-lookat-function

			void lookAt(const Vector& p_source, const Vector& p_dest)
			{
				Vector forwardVector(p_dest[0] - p_source[0],
										p_dest[1] - p_source[1],
										p_dest[2] - p_source[2],
										0.0);

				forwardVector.Normalize();
				Vector forward(0.0, 0.0, -1.0, 0.0);

				const auto dot{ forward * forwardVector };

				if (Maths::abs(dot - (-1.0)) < 0.000001)
				{
					m_quat[0] = 0.0;
					m_quat[1] = 1.0;
					m_quat[2] = 0.0;
					m_quat[3] = Maths::pi;
				}
				else if (Maths::abs(dot - (1.0)) < 0.000001)
				{
					identity();
				}
				else
				{
					const auto rotAngle{ std::acos(dot) };			
					auto rotAxis{ ProdVec(forward, forwardVector) };
					rotAxis.Normalize();
			
					rotationAxis(rotAxis, rotAngle);
				}
			}

			void rotationAxis( Vector& p_axis, dsreal p_angle )
			{
				p_axis.Normalize();

				const auto sin_a{ std::sin(p_angle / 2.0) };

				m_quat[0] = p_axis[0] * sin_a;
				m_quat[1] = p_axis[1] * sin_a;
				m_quat[2] = p_axis[2] * sin_a;
				m_quat[3] = std::cos( p_angle / 2.0 );
			};

			void rotationMatFrom( Matrix& p_mat ) const
			{
				const auto xx{ m_quat[0] * m_quat[0] };
				const auto xy{ m_quat[0] * m_quat[1] };
				const auto xz{ m_quat[0] * m_quat[2] };
				const auto xw{ m_quat[0] * m_quat[3] };
				const auto yy{ m_quat[1] * m_quat[1] };
				const auto yz{ m_quat[1] * m_quat[2] };
				const auto yw{ m_quat[1] * m_quat[3] };
				const auto zz{ m_quat[2] * m_quat[2] };
				const auto zw{ m_quat[2] * m_quat[3] };

				p_mat( 0, 0 ) = 1 - 2 * ( yy + zz );
				p_mat( 1, 0 ) =     2 * ( xy - zw );
				p_mat( 2, 0 ) =     2 * ( xz + yw );
				p_mat( 0, 1 ) =     2 * ( xy + zw );
				p_mat( 1, 1 ) = 1 - 2 * ( xx + zz );
				p_mat( 2, 1 ) =     2 * ( yz - xw );
				p_mat( 0, 2 ) =     2 * ( xz - yw );
				p_mat( 1, 2 ) =     2 * ( yz + xw );
				p_mat( 2, 2 ) = 1 - 2 * ( xx + yy );
				p_mat( 3, 0 ) = p_mat( 3, 1 ) = p_mat( 3, 2 ) = p_mat( 0, 3 ) = p_mat( 1, 3 ) = p_mat( 2, 3 ) = 0;
				p_mat( 3, 3 ) = 1;
			};

			void normalize(void)
			{
				const auto a{ m_quat[0] };
				const auto b{ m_quat[1] };
				const auto c{ m_quat[2] };
				const auto d{ m_quat[3] };

				const auto n{ std::sqrt(a * a + b * b + c * c + d * d) };

				m_quat[0] = a / n;
				m_quat[1] = b / n;
				m_quat[2] = c / n;
				m_quat[3] = d / n;
			}

			static Quaternion lerp(const Quaternion& p_q1, const Quaternion& p_q2, dsreal p_blend);

		private:
			dsreal      m_quat[4];

		};
	}
}

DrawSpace::Maths::Quaternion operator* ( DrawSpace::Maths::Quaternion p_qA, DrawSpace::Maths::Quaternion p_qB );

