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

			void zero(void);
			void identity(void);		
			void lookAt(const Vector& p_source, const Vector& p_dest);
			void rotationAxis(Vector& p_axis, dsreal p_angle);
			void rotationMatFrom(Matrix& p_mat) const;
			void normalize(void);

			static Quaternion lerp(const Quaternion& p_q1, const Quaternion& p_q2, dsreal p_blend);

		private:
			dsreal      m_quat[4];

		};
	}
}

DrawSpace::Maths::Quaternion operator* ( DrawSpace::Maths::Quaternion p_qA, DrawSpace::Maths::Quaternion p_qB );

