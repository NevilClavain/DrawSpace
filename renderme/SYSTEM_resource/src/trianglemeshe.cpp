
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

#include "trianglemeshe.h"

using namespace renderMe;

TriangleMeshe::TriangleMeshe(const std::string& p_name, State p_initial_state) :
	m_name(p_name),
	m_state(p_initial_state)
{
}

TriangleMeshe::TriangleMeshe(const TriangleMeshe& p_other)
{
	m_name = p_other.m_name;
	m_vertices = p_other.m_vertices;
	m_triangles_for_vertex = p_other.m_triangles_for_vertex;

	m_normales_transformation = p_other.m_normales_transformation;
	m_n_gen_mode = p_other.m_n_gen_mode;
	m_tb_gen_mode = p_other.m_tb_gen_mode;

	m_state_mutex.lock();
	p_other.m_state_mutex.lock();
	m_state = p_other.m_state;
	p_other.m_state_mutex.unlock();
	m_state_mutex.unlock();
}

std::string TriangleMeshe::getName(void) const
{
	return m_name;
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
}

void TriangleMeshe::push(const Vertex& p_vertex)
{
	m_vertices.push_back(p_vertex);
}

void TriangleMeshe::push(const TrianglePrimitive<unsigned int>& p_triangle)
{
	m_triangles.push_back(p_triangle);
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
}