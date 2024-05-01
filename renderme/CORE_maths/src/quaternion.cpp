
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

Quaternion::Quaternion()
{
	identity();
}

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


void Quaternion::rotationAxis(const Real3Vector& p_axis, double p_angle)
{
	auto axis{ p_axis };
	axis.normalize();

	const double sin_a{ std::sin(p_angle / 2.0) };

	m_quat[0] = axis[0] * sin_a;
	m_quat[1] = axis[1] * sin_a;
	m_quat[2] = axis[2] * sin_a;
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

// p_forward : vector from destination to source !
Quaternion Quaternion::lookRotation(const Real3Vector& p_forward, const Real3Vector& p_up)
{
	// from quaternion implementation https://gist.github.com/HelloKitty/91b7af87aac6796c3da9
		
	Quaternion result;

	auto forward = p_forward;
	forward.normalize();

	const auto right{ Real3Vector::crossProduct(p_up, forward) };

	const auto up{ Real3Vector::crossProduct(forward, right) };

	const double m00{ right[0] };
	const double m01{ right[1] };
	const double m02{ right[2] };
	const double m10{ up[0]};
	const double m11{ up[1] };
	const double m12{ up[2] };
	const double m20{ forward[0] };
	const double m21{ forward[1] };
	const double m22{ forward[2] };

	const double num8{ (m00 + m11) + m22 };
	if (num8 > 0.0)
	{
		double num = std::sqrt(num8 + 1.0);
		result[3] = num * 0.5;
		num = 0.5 / num;
		result[0] = (m12 - m21) * num;
		result[1] = (m20 - m02) * num;
		result[2] = (m01 - m10) * num;
		return result;
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		const double num7{ std::sqrt(((1.0 + m00) - m11) - m22) };
		const double num4{ 0.5 / num7 };
		result[0] = 0.5 * num7;
		result[1] = (m01 + m10) * num4;
		result[2] = (m02 + m20) * num4;
		result[3] = (m12 - m21) * num4;
		return result;
	}
	if (m11 > m22)
	{
		const double num6{ std::sqrt(((1.0 + m11) - m00) - m22) };
		const double num3{ 0.5 / num6 };
		result[0] = (m10 + m01) * num3;
		result[1] = 0.5 * num6;
		result[2] = (m21 + m12) * num3;
		result[3] = (m20 - m02) * num3;
		return result;
	}

	const double num5{ std::sqrt(((1.0 + m22) - m00) - m11) };
	const double num2{ 0.5 / num5 };
	result[0] = (m20 + m02) * num2;
	result[1] = (m21 + m12) * num2;
	result[2] = 0.5 * num5;
	result[3] = (m01 - m10) * num2;
	return result;
	
}


Quaternion renderMe::core::maths::operator* (const Quaternion& p_qA, const Quaternion& p_qB)
{
	Quaternion res;

	res[0] = p_qB[3] * p_qA[0] + p_qB[0] * p_qA[3] + p_qB[1] * p_qA[2] - p_qB[2] * p_qA[1];
	res[1] = p_qB[3] * p_qA[1] - p_qB[0] * p_qA[2] + p_qB[1] * p_qA[3] + p_qB[2] * p_qA[0];
	res[2] = p_qB[3] * p_qA[2] + p_qB[0] * p_qA[1] - p_qB[1] * p_qA[0] + p_qB[2] * p_qA[3];
	res[3] = p_qB[3] * p_qA[3] - p_qB[0] * p_qA[0] - p_qB[1] * p_qA[1] - p_qB[2] * p_qA[2];

	return res;
}
