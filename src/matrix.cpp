/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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
/* -*-LIC_END-*- */


#include "matrix.h"

using namespace DrawSpace::Utils;

Matrix::Matrix( void )
{
    Zero();
}

Matrix::~Matrix( void )
{
}

void Matrix::Inverse( void )
{
    Matrix t;
    Matrix res;

    t.Translation( -m_matrix[3][0], -m_matrix[3][1], -m_matrix[3][2] );

    ClearTranslation();
    Transpose();
    MatrixMult( &t, this, &res );

    memcpy( &m_matrix, &res.m_matrix, 16 * sizeof( dsreal ) );
}

void Matrix::Rotation( const Vector& p_axis, dsreal p_angle )
{
    dsreal c = std::cos( p_angle );
    dsreal s = std::sin( p_angle );
    dsreal omc = (1 - c);

    Vector axis = p_axis;

    axis.Normalize();

    dsreal x = axis[0];
    dsreal y = axis[1];
    dsreal z = axis[2];	
    dsreal xs = x * s;
    dsreal ys = y * s;
    dsreal zs = z * s;
    dsreal xyomc = x * y * omc;
    dsreal xzomc = x * z * omc;
    dsreal yzomc = y * z * omc;

    m_matrix[0][0] = x * x * omc + c;
    m_matrix[0][1] = xyomc + zs;
    m_matrix[0][2] = xzomc - ys;
    m_matrix[0][3] = 0;
    
    m_matrix[1][0] = xyomc - zs;
    m_matrix[1][1] = y * y * omc + c;
    m_matrix[1][2] = yzomc + xs;
    m_matrix[1][3] = 0;
    
    m_matrix[2][0] = xzomc + ys;
    m_matrix[2][1] = yzomc - xs;
    m_matrix[2][2] = z * z * omc + c;
    m_matrix[2][3] = 0;
    
    m_matrix[3][0] = 0;
    m_matrix[3][1] = 0;
    m_matrix[3][2] = 0;
    m_matrix[3][3] = 1;

    m_configinfos.type = CONFIG_ROTATION;

    m_configinfos.values = p_axis;
    m_configinfos.values[3] = p_angle;
}

void Matrix::Transform( Vector* p_vec_in, Vector* p_vec_out ) const
{
    (*p_vec_out)[0] = (*p_vec_in)[0] * m_matrix[0][0] +
                      (*p_vec_in)[1] * m_matrix[1][0] +
                      (*p_vec_in)[2] * m_matrix[2][0] + 
                      (*p_vec_in)[3] * m_matrix[3][0];
                      
    (*p_vec_out)[1] = (*p_vec_in)[0] * m_matrix[0][1] +
                      (*p_vec_in)[1] * m_matrix[1][1] +
                      (*p_vec_in)[2] * m_matrix[2][1] +
                      (*p_vec_in)[3] * m_matrix[3][1];

    (*p_vec_out)[2] = (*p_vec_in)[0] * m_matrix[0][2] +
                      (*p_vec_in)[1] * m_matrix[1][2] +
                      (*p_vec_in)[2] * m_matrix[2][2] +
                      (*p_vec_in)[3] * m_matrix[3][2];
}


void Matrix::MatrixMult( Matrix* p_mA, Matrix* p_mB, Matrix* p_mRes )
{
    int i, j;
    for( i = 0; i < 4; i++)
    {
        for( j = 0; j < 4; j++ )
        {
            p_mRes->m_matrix[i][j] = p_mA->m_matrix[i][0] * p_mB->m_matrix[0][j] +
                                     p_mA->m_matrix[i][1] * p_mB->m_matrix[1][j] +
                                     p_mA->m_matrix[i][2] * p_mB->m_matrix[2][j] +
                                     p_mA->m_matrix[i][3] * p_mB->m_matrix[3][j];
        }
    }

    p_mRes->m_configinfos.type = CONFIG_UNDETERMINED;
}


Matrix operator* ( Matrix p_mA, Matrix p_mB )
{
    Matrix res;
    int i, j;
    for( i = 0; i < 4; i++)
    {
        for( j = 0; j < 4; j++ )
        {
            res( i, j ) = p_mA( i, 0 ) * p_mB( 0, j ) +
                          p_mA( i, 1 ) * p_mB( 1, j ) +
                          p_mA( i, 2 ) * p_mB( 2, j ) +
                          p_mA( i, 3 ) * p_mB( 3, j );
        }
    }
    return res;
}




