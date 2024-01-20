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

#include <vector>
#include "matrix.h"

namespace renderMe
{
	namespace transform
	{
		class MatrixChain
		{
		public:
			MatrixChain();
			MatrixChain(int p_nbmat);
			~MatrixChain() = default;

			void    pushMatrix(const renderMe::core::maths::Matrix& p_matrix);
			void    updateMatrix(size_t p_index, const renderMe::core::maths::Matrix& p_mat);
			void	reset();

			void    buildResult(void);
			void    getResultTransform(renderMe::core::maths::Matrix& p_mat) const;

		private:
			std::vector<renderMe::core::maths::Matrix>	m_matrix_chain;
			renderMe::core::maths::Matrix	            m_result;
		};
	}
}
