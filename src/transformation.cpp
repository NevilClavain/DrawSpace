/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
*                                                                          
* This file is part of DrawSpace.                                          
*                                                                          
*    DrawSpace is free software: you can redistribute it and/or modify     
*    it under the terms of the GNU General Public License as published by  
*    the Free Software Foundation, either version 3 of the License, or     
*    (at your option) any later version.                                   
*                                                                          
*    DrawSpace is distributed in the hope that it will be useful,          
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
*    GNU General Public License for more details.                          
*                                                                          
*    You should have received a copy of the GNU General Public License     
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    
*
*/

#include "transformation.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Transformation::Transformation( void )
{
    m_result.Identity();
}

Transformation::Transformation( int p_nbmat )
{
    m_result.Identity();
    for( int i = 0; i < p_nbmat; i++ )
    {
        Matrix ident;
        ident.Identity();
        PushMatrix( ident );
    }
}

Transformation::~Transformation( void )
{
}

void Transformation::PushMatrix( const Matrix& p_matrix )
{
    m_matrix_chain.push_back( p_matrix );
}

// operation : pour une liste de N matrices, BuildResult fait :
// [N] * [N-1] * [N - 2] * ..... * [2] * [1] * [0]

// la 1ERE matrice appliquee est la matrice N

void Transformation::BuildResult( void )
{
    if( m_matrix_chain.size() > 0 )
    {
        Matrix stack = m_matrix_chain[0];
        for( unsigned long i = 1; i < m_matrix_chain.size(); i++ )
        {
            Matrix res, current;

            current = m_matrix_chain[i];
            Matrix::MatrixMult( &current, &stack, &res );
            stack = res;
        }
        m_result = stack;
    }
}

void Transformation::GetResult( Matrix* p_res )
{
    *p_res = m_result;
}

void Transformation::ClearAll( void )
{
    m_result.Identity();
    m_matrix_chain.clear();
}

void Transformation::GetMatrixChain( std::vector<DrawSpace::Utils::Matrix>& p_list )
{
    p_list = m_matrix_chain;
}

void Transformation::UpdateMatrix( size_t p_index, const Matrix& p_mat )
{
    if( p_index >= m_matrix_chain.size() )
    {
        _DSEXCEPTION( "index out of range" );
    }

    m_matrix_chain[p_index] = p_mat;
}

void Transformation::Update( DrawSpace::Utils::TimeManager& p_timemanager )
{    
    BuildResult();
}

void Transformation::GetBaseTransform( DrawSpace::Utils::Matrix& p_mat )
{
    p_mat = m_result;
}

