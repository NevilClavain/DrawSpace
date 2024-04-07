
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

#include "quaternion.h"

using namespace renderMe::core::maths;

Quaternion::Quaternion(double p_x, double p_y, double p_z, double p_w)
{
	m_quat[0] = p_x;
	m_quat[1] = p_y;
	m_quat[2] = p_z;
	m_quat[3] = p_w;
}

void Quaternion::zero(void)
{
	m_quat[0] = 0.0;
	m_quat[1] = 0.0;
	m_quat[2] = 0.0;
	m_quat[3] = 0.0;
}

void Quaternion::identity(void)
{
	m_quat[0] = 0.0;
	m_quat[1] = 0.0;
	m_quat[2] = 0.0;
	m_quat[3] = 1.0;
}


// https://stackoverflow.com/questions/12435671/quaternion-lookat-function

void Quaternion::lookAt(const Real4Vector& p_source, const Real4Vector& p_dest)
{
	Real4Vector forwardVector(p_dest[0] - p_source[0],
									p_dest[1] - p_source[1],
									p_dest[2] - p_source[2],
									0.0);

	forwardVector.normalize();
	Real4Vector forward(0.0, 0.0, -1.0, 0.0);

	const auto dot{ forward * forwardVector };

	if (std::abs(dot - (-1.0)) < 0.000001)
	{
		m_quat[0] = 0.0;
		m_quat[1] = 1.0;
		m_quat[2] = 0.0;
		m_quat[3] = 3.1415927;
	}
	else if (std::abs(dot - (1.0)) < 0.000001)
	{
		identity();
	}
	else
	{
		const auto rotAngle{ std::acos(dot) };
		auto rotAxis{ Real4Vector::crossProduct(forward, forwardVector) };
		rotAxis.normalize();

		rotationAxis(rotAxis, rotAngle);
	}
}

void Quaternion::rotationAxis(Real4Vector& p_axis, double p_angle)
{
	p_axis.normalize();

	const double sin_a{ std::sin(p_angle / 2.0) };

	m_quat[0] = p_axis[0] * sin_a;
	m_quat[1] = p_axis[1] * sin_a;
	m_quat[2] = p_axis[2] * sin_a;
	m_quat[3] = std::cos(p_angle / 2.0);
}

void Quaternion::rotationMatFrom(Matrix& p_mat) const
{
	const double xx{ m_quat[0] * m_quat[0] };
	const double xy{ m_quat[0] * m_quat[1] };
	const double xz{ m_quat[0] * m_quat[2] };
	const double xw{ m_quat[0] * m_quat[3] };
	const double yy{ m_quat[1] * m_quat[1] };
	const double yz{ m_quat[1] * m_quat[2] };
	const double yw{ m_quat[1] * m_quat[3] };
	const double zz{ m_quat[2] * m_quat[2] };
	const double zw{ m_quat[2] * m_quat[3] };

	p_mat(0, 0) = 1 - 2 * (yy + zz);
	p_mat(1, 0) = 2 * (xy - zw);
	p_mat(2, 0) = 2 * (xz + yw);
	p_mat(0, 1) = 2 * (xy + zw);
	p_mat(1, 1) = 1 - 2 * (xx + zz);
	p_mat(2, 1) = 2 * (yz - xw);
	p_mat(0, 2) = 2 * (xz - yw);
	p_mat(1, 2) = 2 * (yz + xw);
	p_mat(2, 2) = 1 - 2 * (xx + yy);
	p_mat(3, 0) = p_mat(3, 1) = p_mat(3, 2) = p_mat(0, 3) = p_mat(1, 3) = p_mat(2, 3) = 0;
	p_mat(3, 3) = 1;
}

void Quaternion::normalize(void)
{
	const double a{ m_quat[0] };
	const double b{ m_quat[1] };
	const double c{ m_quat[2] };
	const double d{ m_quat[3] };

	const double n{ std::sqrt(a * a + b * b + c * c + d * d) };

	m_quat[0] = a / n;
	m_quat[1] = b / n;
	m_quat[2] = c / n;
	m_quat[3] = d / n;
}

Quaternion Quaternion::lerp(const Quaternion& p_q1, const Quaternion& p_q2, double p_blend)
{
	Quaternion q1 { p_q1 };
	Quaternion q2 = p_q2;
	Quaternion result;

	q1.normalize();
	q2.normalize();

	const double dot_product{ p_q1[0] * p_q2[0] + p_q1[1] * p_q2[1] + p_q1[2] * p_q2[2] + p_q1[3] * p_q2[3] };
	const double one_minus_blend{ 1.0 - p_blend };

	const double sign{ dot_product < 0.0 ? -1.0 : 1.0 };

	result[0] = p_q1[0] * one_minus_blend + p_blend * sign * p_q2[0];
	result[1] = p_q1[1] * one_minus_blend + p_blend * sign * p_q2[1];
	result[2] = p_q1[2] * one_minus_blend + p_blend * sign * p_q2[2];
	result[3] = p_q1[3] * one_minus_blend + p_blend * sign * p_q2[3];

	result.normalize();
	return result;
}

Quaternion operator* (Quaternion p_qA, Quaternion p_qB)
{
	Quaternion res;

	res[0] = p_qB[3] * p_qA[0] + p_qB[0] * p_qA[3] + p_qB[1] * p_qA[2] - p_qB[2] * p_qA[1];
	res[1] = p_qB[3] * p_qA[1] - p_qB[0] * p_qA[2] + p_qB[1] * p_qA[3] + p_qB[2] * p_qA[0];
	res[2] = p_qB[3] * p_qA[2] + p_qB[0] * p_qA[1] - p_qB[1] * p_qA[0] + p_qB[2] * p_qA[3];
	res[3] = p_qB[3] * p_qA[3] - p_qB[0] * p_qA[0] - p_qB[1] * p_qA[1] - p_qB[2] * p_qA[2];

	return res;
}