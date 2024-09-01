
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

#include <md5.h>
#include "trianglemeshe.h"


using namespace renderMe;
using namespace renderMe::core::maths;

TriangleMeshe::TriangleMeshe(const TriangleMeshe& p_other)
{
	m_vertices = p_other.m_vertices;
	m_triangles = p_other.m_triangles;
	m_triangles_for_vertex = p_other.m_triangles_for_vertex;

	m_normales_transformation = p_other.m_normales_transformation;
	m_n_gen_mode = p_other.m_n_gen_mode;
	m_tb_gen_mode = p_other.m_tb_gen_mode;

	m_state_mutex.lock();
	p_other.m_state_mutex.lock();
	m_state = p_other.m_state;
	p_other.m_state_mutex.unlock();
	m_state_mutex.unlock();

	m_md5 = p_other.m_md5;
}


std::vector<renderMe::Vertex>TriangleMeshe::getVertices(void) const
{
	return m_vertices;
}

size_t TriangleMeshe::getVerticesListSize() const
{
	return m_vertices.size();
}


std::vector<TrianglePrimitive<unsigned int>> TriangleMeshe::getTriangles(void) const
{
	return m_triangles;
}

size_t TriangleMeshe::getTrianglesListSize() const
{
	return m_triangles.size();
}


void TriangleMeshe::setNGenerationMode(NormalesGenerationMode p_mode)
{
	m_n_gen_mode = p_mode;
}

void TriangleMeshe::setTBGenerationMode(TangentBinormalesGenerationMode p_mode)
{
	m_tb_gen_mode = p_mode;
}

TriangleMeshe::NormalesGenerationMode TriangleMeshe::getNGenerationMode(void) const
{
	return m_n_gen_mode;
}

TriangleMeshe::TangentBinormalesGenerationMode TriangleMeshe::getTBGenerationMode(void) const
{
	return m_tb_gen_mode;
}

core::maths::Matrix	TriangleMeshe::getNormalesTransf(void) const
{
	return m_normales_transformation;
}

void TriangleMeshe::setNormalesTransf(const core::maths::Matrix& p_transf)
{
	m_normales_transformation = p_transf;
}

void TriangleMeshe::clearVertices(void)
{
	m_vertices.clear();
}

void TriangleMeshe::clearTriangles(void)
{
	m_triangles.clear();
	m_triangles_for_vertex.clear();
}

void TriangleMeshe::push(const Vertex& p_vertex)
{
	m_vertices.push_back(p_vertex);
}

void TriangleMeshe::push(const TrianglePrimitive<unsigned int>& p_triangle)
{
	m_triangles.push_back(p_triangle);

	m_triangles_for_vertex[p_triangle[0]].push_back(p_triangle);
	m_triangles_for_vertex[p_triangle[1]].push_back(p_triangle);
	m_triangles_for_vertex[p_triangle[2]].push_back(p_triangle);

}

void TriangleMeshe::computeNormales()
{
	for (auto it = m_triangles_for_vertex.begin(); it != m_triangles_for_vertex.end(); ++it)
	{
		Real4Vector normales_sum;
		const auto triangles_list{ it->second };

		for (size_t i = 0; i < triangles_list.size(); i++)
		{
			const auto triangle{ triangles_list.at(i) };
			const Vertex v1 { m_vertices.at(triangle[0]) };
			const Vertex v2 { m_vertices.at(triangle[1]) };
			const Vertex v3 { m_vertices.at(triangle[2]) };

			const Vector d1(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z, 1.0);
			const Vector d2(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z, 1.0);

			Vector res { Real4Vector::crossProduct(d1, d2) };
			res.normalize();

			normales_sum = normales_sum + res;
		}
		normales_sum.scale(1.0 / triangles_list.size());
		normales_sum.normalize();

		m_vertices[it->first].nx = normales_sum[0];
		m_vertices[it->first].ny = normales_sum[1];
		m_vertices[it->first].nz = normales_sum[2];
	}
}

void TriangleMeshe::compute_TBN(const Vertex& p_v1, const Vertex& p_v2, const Vertex& p_v3, int p_stage,
	core::maths::Real4Vector& p_T, core::maths::Real4Vector& p_B, core::maths::Real4Vector& p_N)
{
	const Real4Vector v2v1(p_v2.x - p_v1.x, p_v2.y - p_v1.y, p_v2.z - p_v1.z, 1.0);
	const Real4Vector v3v1(p_v3.x - p_v1.x, p_v3.y - p_v1.y, p_v3.z - p_v1.z, 1.0);

	const double c2c1t { p_v2.tu[p_stage] - p_v1.tu[p_stage] };
	const double c2c1b { p_v2.tv[p_stage] - p_v1.tv[p_stage] };

	const double c3c1t { p_v3.tu[p_stage] - p_v1.tu[p_stage] };
	const double c3c1b { p_v3.tv[p_stage] - p_v1.tv[p_stage] };

	const double det { (c2c1t * c3c1b) - (c3c1t * c2c1b) };

	Vector t, b, n;

	t[0] = ((c3c1b * v2v1[0]) - (c2c1b * v3v1[0])) / det;
	t[1] = ((c3c1b * v2v1[1]) - (c2c1b * v3v1[1])) / det;
	t[2] = ((c3c1b * v2v1[2]) - (c2c1b * v3v1[2])) / det;
	t[3] = 1.0;

	b[0] = ((-c3c1t * v2v1[0]) + (c2c1t * v3v1[0])) / det;
	b[1] = ((-c3c1t * v2v1[1]) + (c2c1t * v3v1[1])) / det;
	b[2] = ((-c3c1t * v2v1[2]) + (c2c1t * v3v1[2])) / det;
	b[3] = 1.0;

	n = Real4Vector::crossProduct(b, t);

	t.normalize();
	b.normalize();
	n.normalize();

	p_T = t;
	p_B = b;
	p_N = n;
}

void TriangleMeshe::computeTB()
{
	for (auto it = m_triangles_for_vertex.begin(); it != m_triangles_for_vertex.end(); ++it)
	{
		Real4Vector tangents_sum;
		Real4Vector binormales_sum;
		Real4Vector normales_sum;

		const auto triangles_list{ it->second };

		for (size_t i = 0; i < triangles_list.size(); i++)
		{
			const auto triangle{ triangles_list.at(i) };
			const Vertex v1 { m_vertices.at(triangle[0]) };
			const Vertex v2 { m_vertices.at(triangle[1]) };
			const Vertex v3 { m_vertices.at(triangle[2]) };

			Real4Vector t, b, n;
			compute_TBN(v1, v2, v3, 0, t, b, n);

			normales_sum = normales_sum + n;
			binormales_sum = binormales_sum + b;
			tangents_sum = tangents_sum + t;
		}

		normales_sum.scale(1.0 / triangles_list.size());
		normales_sum.normalize();

		binormales_sum.scale(1.0 / triangles_list.size());
		binormales_sum.normalize();

		tangents_sum.scale(1.0 / triangles_list.size());
		tangents_sum.normalize();

		m_vertices[it->first].bx = binormales_sum[0];
		m_vertices[it->first].by = binormales_sum[1];
		m_vertices[it->first].bz = binormales_sum[2];

		m_vertices[it->first].tx = tangents_sum[0];
		m_vertices[it->first].ty = tangents_sum[1];
		m_vertices[it->first].tz = tangents_sum[2];
	}
}


TriangleMeshe::State TriangleMeshe::getState() const
{
	m_state_mutex.lock();
	const auto state{ m_state };
	m_state_mutex.unlock();
	return state;
}

void TriangleMeshe::setState(TriangleMeshe::State p_state)
{
	m_state_mutex.lock();
	m_state = p_state;
	m_state_mutex.unlock();

	if (State::BLOBLOADED == m_state)
	{
		compute_md5();
	}
}

void TriangleMeshe::compute_md5()
{
	MD5 md5;

	const auto vbuff{ new Vertex[m_vertices.size()] };
	auto curr{ vbuff };
	for (size_t i = 0; i < m_vertices.size(); i++)
	{
		*curr = m_vertices[i];
		curr++;
	}
	const std::string hash_v{ md5.digestMemory((BYTE*)vbuff, (int)(m_vertices.size() * sizeof(Vertex))) };

	auto tbuff{ new TrianglePrimitive<unsigned int>[m_triangles.size()] };
	TrianglePrimitive<unsigned int>* curr2{ tbuff };

	for (size_t i = 0; i < m_triangles.size(); i++)
	{
		const TrianglePrimitive<unsigned int> triangle{ m_triangles.at(i) };
		*curr2 = triangle;
		curr2++;
	}
	const std::string hash_t{ md5.digestMemory((BYTE*)tbuff, (int)(m_triangles.size() * sizeof(TrianglePrimitive<unsigned int>))) };

	delete[] vbuff;
	delete[] tbuff;

	const std::string hash_n_gen{ md5.digestMemory((BYTE*)&m_n_gen_mode, (int)(sizeof(m_n_gen_mode))) };
	const std::string hash_tb_gen{ md5.digestMemory((BYTE*)&m_tb_gen_mode, (int)(sizeof(m_tb_gen_mode))) };

	std::string hash{ hash_v + hash_t + hash_n_gen + hash_tb_gen };
	
	m_md5 = hash;
}

std::string	TriangleMeshe::getMd5() const
{
	return m_md5;
}