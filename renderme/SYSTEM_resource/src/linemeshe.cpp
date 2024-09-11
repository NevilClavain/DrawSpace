
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
#include "linemeshe.h"

using namespace renderMe;

LineMeshe::LineMeshe(const LineMeshe& p_other)
{
	m_source_id = p_other.m_source_id;
	m_resource_uid = p_other.m_resource_uid;

	m_vertices = p_other.m_vertices;
	m_lines = p_other.m_lines;

	m_state_mutex.lock();
	p_other.m_state_mutex.lock();
	m_state = p_other.m_state;
	p_other.m_state_mutex.unlock();
	m_state_mutex.unlock();
}


std::vector<renderMe::Vertex>LineMeshe::getVertices(void) const
{
	return m_vertices;
}

std::vector<LinePrimitive<unsigned int>> LineMeshe::getLines(void) const
{
	return m_lines;
}

void LineMeshe::clearLines(void)
{
	m_lines.clear();
}

void LineMeshe::clearVertices(void)
{
	m_vertices.clear();
}

void LineMeshe::push(const LinePrimitive<unsigned int>& p_line)
{
	m_lines.push_back(p_line);
}

void LineMeshe::push(const Vertex& p_vertex)
{
	m_vertices.push_back(p_vertex);
}

size_t LineMeshe::getVerticesListSize() const
{
	return m_vertices.size();
}

size_t LineMeshe::getLinesListSize() const
{
	return m_lines.size();
}


LineMeshe::State LineMeshe::getState() const
{
	m_state_mutex.lock();
	const auto state{ m_state };
	m_state_mutex.unlock();
	return state;
}

void LineMeshe::setState(LineMeshe::State p_state)
{
	m_state_mutex.lock();
	m_state = p_state;
	m_state_mutex.unlock();
}


void LineMeshe::computeResourceUID()
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

	auto lbuff{ new LinePrimitive<unsigned int>[m_lines.size()] };
	LinePrimitive<unsigned int>* curr2{ lbuff };

	for (size_t i = 0; i < m_lines.size(); i++)
	{
		const LinePrimitive<unsigned int> line{ m_lines.at(i) };
		*curr2 = line;
		curr2++;
	}
	const std::string hash_t{ md5.digestMemory((BYTE*)lbuff, (int)(m_lines.size() * sizeof(TrianglePrimitive<unsigned int>))) };

	delete[] vbuff;
	delete[] lbuff;

	std::string hash{ hash_v + hash_t };

	m_resource_uid = hash;
}

std::string LineMeshe::getResourceUID() const
{
	return m_resource_uid;
}

std::string LineMeshe::getSourceID() const
{
	return m_source_id;
}

void LineMeshe::setSourceID(const std::string& p_source_id)
{
	m_source_id = p_source_id;
}
