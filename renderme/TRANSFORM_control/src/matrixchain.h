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

#include <vector>
#include "matrix.h"

namespace mage
{
	namespace transform
	{
		class MatrixChain
		{
		public:
			MatrixChain();
			MatrixChain(int p_nbmat);
			~MatrixChain() = default;

			void					pushMatrix(const mage::core::maths::Matrix& p_matrix);
			void					updateMatrix(size_t p_index, const mage::core::maths::Matrix& p_mat);
			void					reset();

			void					buildResult(void);
			core::maths::Matrix		getResultTransform() const;

		private:
			std::vector<mage::core::maths::Matrix>	m_matrix_chain;
			mage::core::maths::Matrix	            m_result;
		};
	}
}
