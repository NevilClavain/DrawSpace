
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

#include "linemeshe.h"

using namespace renderMe;

LineMeshe::LineMeshe(const std::string& p_name, State p_initial_state) :
m_name(p_name),
m_state(p_initial_state)
{
}

LineMeshe::LineMeshe(const LineMeshe& p_other)
{
	m_name = p_other.m_name;
	m_vertices = p_other.m_vertices;
	m_lines = p_other.m_lines;

	m_state_mutex.lock();
	p_other.m_state_mutex.lock();
	m_state = p_other.m_state;
	p_other.m_state_mutex.unlock();
	m_state_mutex.unlock();
}

std::string LineMeshe::getName(void) const
{
	return m_name;
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
