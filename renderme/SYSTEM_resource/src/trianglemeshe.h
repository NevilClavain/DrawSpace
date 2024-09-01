
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
#include <map>

#include "primitives.h"
#include "matrix.h"
#include "tvector.h"

namespace renderMe
{
	class TriangleMeshe
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

		enum class NormalesGenerationMode
		{
			NORMALES_DISCARDED,
			NORMALES_AUTO,
			NORMALES_AUTO_SMOOTH,
			NORMALES_FROMFILE,
			NORMALES_FROMFILE_SMOOTH,
			NORMALES_COMPUTED
		};

		enum class TangentBinormalesGenerationMode
		{
			TB_DISCARDED,
			TB_AUTO,
			TB_FROMFILE,
			TB_COMPUTED
		};

		TriangleMeshe() = default;		
		TriangleMeshe(const TriangleMeshe& p_other);

		TriangleMeshe& operator=(const TriangleMeshe& p_other)
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

			return *this;
		}

		~TriangleMeshe() = default;

		

		std::vector<renderMe::Vertex>					getVertices(void) const;
		size_t											getVerticesListSize() const;
		
		std::vector<TrianglePrimitive<unsigned int>>	getTriangles(void) const;
		size_t											getTrianglesListSize() const;

		void											setNGenerationMode(NormalesGenerationMode p_mode);
		void											setTBGenerationMode(TangentBinormalesGenerationMode p_mode);

		NormalesGenerationMode							getNGenerationMode(void) const;
		TangentBinormalesGenerationMode					getTBGenerationMode(void) const;

		core::maths::Matrix								getNormalesTransf(void) const;
		void											setNormalesTransf(const core::maths::Matrix& p_transf);


		void											clearVertices(void);
		void											clearTriangles(void);

		void											push(const TrianglePrimitive<unsigned int>& p_triangle);
		void											push(const Vertex& p_vertex);

		void											computeNormales();
		void											computeTB();


		State											getState() const;
		void											setState(State p_state);

		

		std::string										getMd5() const;


	private:

		std::vector<Vertex>												m_vertices;
		std::vector<TrianglePrimitive<unsigned int>>					m_triangles;

		// list of triangles for each vertex
		std::map<long, std::vector<TrianglePrimitive<unsigned int>>>	m_triangles_for_vertex;

		NormalesGenerationMode											m_n_gen_mode{ NormalesGenerationMode::NORMALES_COMPUTED };
		TangentBinormalesGenerationMode									m_tb_gen_mode{ TangentBinormalesGenerationMode::TB_DISCARDED };

		core::maths::Matrix												m_normales_transformation;

		mutable std::mutex												m_state_mutex;
		State															m_state{ State::INIT };

		std::string														m_md5;

		// IF NEW MEMBERS HERE :
		// UPDATE COPY CTOR AND OPERATOR !!!!!!

		void compute_TBN(const Vertex& p_v1, const Vertex& p_v2, const Vertex& p_v3, int p_stage,
							core::maths::Real4Vector& p_T, core::maths::Real4Vector& p_B, core::maths::Real4Vector& p_N);

		void compute_md5();

	};
}
