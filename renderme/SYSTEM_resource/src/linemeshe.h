
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

#include <string>
#include <vector>

#include "primitives.h"

namespace renderMe
{
	class LineMeshe
	{
	public:
		LineMeshe() = delete;
		LineMeshe(const std::string& p_name);
		~LineMeshe() = default;

		std::string getName(void) const;

		std::vector<renderMe::Vertex>				getVertices(void) const;
		std::vector<LinePrimitive<unsigned int>>	getLines(void) const;

		size_t		getLinesListSize() const;
		size_t		getVerticesListSize() const;


		void push(const LinePrimitive<unsigned int>& p_line);
		void push(const Vertex& p_vertex);

		void clearLines(void);
		void clearVertices(void);

		bool isReady() const;
		
	private:
		std::string									m_name;
		std::vector<Vertex>							m_vertices;
		std::vector<LinePrimitive<unsigned int>>    m_lines;

	};

} // renderMe