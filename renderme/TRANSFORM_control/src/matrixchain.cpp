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

#include "matrixchain.h"
#include "exceptions.h"

using namespace renderMe::transform;
using namespace renderMe::core::maths;

MatrixChain::MatrixChain()
{
	m_result.identity();
}

MatrixChain::MatrixChain(int p_nbmat)
{
	m_result.identity();
    for (int i = 0; i < p_nbmat; i++)
    {
        Matrix ident;
        ident.identity();
        pushMatrix(ident);
    }
}

void MatrixChain::pushMatrix(const renderMe::core::maths::Matrix& p_matrix)
{
    m_matrix_chain.push_back(p_matrix);
}

void MatrixChain::updateMatrix(size_t p_index, const renderMe::core::maths::Matrix& p_mat)
{
    if (p_index >= m_matrix_chain.size())
    {
        _EXCEPTION("index out of range");
    }
    m_matrix_chain[p_index] = p_mat;
}

void MatrixChain::reset()
{
    m_result.identity();
    m_matrix_chain.clear();
}


// operation : pour une liste de N matrices, BuildResult fait :
// [N] * [N-1] * [N - 2] * ..... * [2] * [1] * [0]

// la 1ERE matrice appliquee est la matrice N

void MatrixChain::buildResult(void)
{
    if (m_matrix_chain.size() > 0)
    {
        Matrix stack{ m_matrix_chain.at(0) };
        for (unsigned long i = 1; i < m_matrix_chain.size(); i++)
        {
            Matrix res;
            Matrix::matrixMult(&m_matrix_chain[i], &stack, &res);
            stack = res;
        }
        m_result = stack;
    }
}

renderMe::core::maths::Matrix MatrixChain::getResultTransform() const
{
    return m_result;
}
