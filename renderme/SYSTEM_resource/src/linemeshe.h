
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
#include <mutex>

#include "primitives.h"

namespace renderMe
{
	class LineMeshe
	{
	public:
		enum class State
		{
			INIT,
			BLOBLOADING,
			BLOBLOADED,
			RENDERERLOADING,
			RENDERERLOADED,
		};

		LineMeshe() = default;
		LineMeshe(const LineMeshe& p_other);

		LineMeshe& operator=(const LineMeshe& p_other)
		{
			m_vertices = p_other.m_vertices;
			m_lines = p_other.m_lines;

			m_state_mutex.lock();
			p_other.m_state_mutex.lock();
			m_state = p_other.m_state;
			p_other.m_state_mutex.unlock();
			m_state_mutex.unlock();

			m_md5 = p_other.m_md5;

			return *this;
		}

		~LineMeshe() = default;

		std::vector<renderMe::Vertex>				getVertices(void) const;
		std::vector<LinePrimitive<unsigned int>>	getLines(void) const;

		size_t		getLinesListSize() const;
		size_t		getVerticesListSize() const;


		void push(const LinePrimitive<unsigned int>& p_line);
		void push(const Vertex& p_vertex);

		void clearLines(void);
		void clearVertices(void);

		State getState() const;
		void setState(State p_state);

		std::string	getMd5() const;
		
	private:

		std::vector<Vertex>							m_vertices;
		std::vector<LinePrimitive<unsigned int>>    m_lines;

		mutable std::mutex							m_state_mutex;
		State										m_state{ State::INIT };

		std::string									m_md5;

		// IF NEW MEMBERS HERE :
		// UPDATE COPY CTOR AND OPERATOR !!!!!!

		void compute_md5();
	};

} // renderMe