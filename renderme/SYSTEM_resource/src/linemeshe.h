
/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#include <string>
#include <vector>
#include <mutex>

#include "primitives.h"

namespace mage
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

			m_source_id = p_other.m_source_id;
			m_resource_uid = p_other.m_resource_uid;

			m_vertices = p_other.m_vertices;
			m_lines = p_other.m_lines;

			m_state_mutex.lock();
			p_other.m_state_mutex.lock();
			m_state = p_other.m_state;
			p_other.m_state_mutex.unlock();
			m_state_mutex.unlock();

			return *this;
		}

		~LineMeshe() = default;

		std::vector<mage::Vertex>				getVertices(void) const;
		std::vector<LinePrimitive<unsigned int>>	getLines(void) const;

		size_t		getLinesListSize() const;
		size_t		getVerticesListSize() const;


		void push(const LinePrimitive<unsigned int>& p_line);
		void push(const Vertex& p_vertex);

		void clearLines(void);
		void clearVertices(void);

		State getState() const;
		void setState(State p_state);

		std::string	getResourceUID() const;
		std::string getSourceID() const;

		void setSourceID(const std::string& p_source_id);

		void computeResourceUID();
		
	private:

		std::string									m_resource_uid;       // meshe content source unique identifier

		std::string									m_source_id;

		std::vector<Vertex>							m_vertices;
		std::vector<LinePrimitive<unsigned int>>    m_lines;

		mutable std::mutex							m_state_mutex;
		State										m_state{ State::INIT };

		// IF NEW MEMBERS HERE :
		// UPDATE COPY CTOR AND OPERATOR !!!!!!
	};

} // mage