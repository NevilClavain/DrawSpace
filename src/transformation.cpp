/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

using namespace DrawSpace::Utils;

Transformation::Transformation( void )
{
    m_result.Identity();
}

Transformation::~Transformation( void )
{
}

void Transformation::PushMatrix( Matrix p_matrix )
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