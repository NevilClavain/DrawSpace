/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

using namespace DrawSpace::Maths;

Matrix::Matrix( void )
{
    zero();
}

void Matrix::zero(void)
{
    m_matrix[0][0] = 0.0;
    m_matrix[0][1] = 0.0;
    m_matrix[0][2] = 0.0;
    m_matrix[0][3] = 0.0;

    m_matrix[1][0] = 0.0;
    m_matrix[1][1] = 0.0;
    m_matrix[1][2] = 0.0;
    m_matrix[1][3] = 0.0;

    m_matrix[2][0] = 0.0;
    m_matrix[2][1] = 0.0;
    m_matrix[2][2] = 0.0;
    m_matrix[2][3] = 0.0;

    m_matrix[3][0] = 0.0;
    m_matrix[3][1] = 0.0;
    m_matrix[3][2] = 0.0;
    m_matrix[3][3] = 0.0;

    m_configinfos.type = ConfigurationType::CONFIG_ZERO;

    m_configinfos.values[0] = 0.0;
    m_configinfos.values[1] = 0.0;
    m_configinfos.values[2] = 0.0;
    m_configinfos.values[3] = 0.0;
}

void Matrix::identity(void)
{
    zero();
    m_matrix[0][0] = 1.0;
    m_matrix[1][1] = 1.0;
    m_matrix[2][2] = 1.0;
    m_matrix[3][3] = 1.0;

    m_configinfos.type = ConfigurationType::CONFIG_IDENTITY;

    m_configinfos.values[0] = 0.0;
    m_configinfos.values[1] = 0.0;
    m_configinfos.values[2] = 0.0;
    m_configinfos.values[3] = 0.0;
}

void Matrix::translation(dsreal p_x, dsreal p_y, dsreal p_z)
{
    identity();
    m_matrix[3][0] = p_x;
    m_matrix[3][1] = p_y;
    m_matrix[3][2] = p_z;

    m_configinfos.type = ConfigurationType::CONFIG_TRANSLATION;
    m_configinfos.values[0] = p_x;
    m_configinfos.values[1] = p_y;
    m_configinfos.values[2] = p_z;
    m_configinfos.values[3] = 0.0;

}

void Matrix::translation(const Maths::Vector& p_pos)
{
    identity();
    m_matrix[3][0] = p_pos[0];
    m_matrix[3][1] = p_pos[1];
    m_matrix[3][2] = p_pos[2];

    m_configinfos.type = ConfigurationType::CONFIG_TRANSLATION;
    m_configinfos.values[0] = p_pos[0];
    m_configinfos.values[1] = p_pos[1];
    m_configinfos.values[2] = p_pos[2];
    m_configinfos.values[3] = 0.0;

}

void Matrix::perspective(dsreal p_w, dsreal p_h, dsreal p_zn, dsreal p_zf)
{
    zero();
    m_matrix[0][0] = 2.0f * p_zn / p_w;
    m_matrix[1][1] = 2.0f * p_zn / p_h;
    m_matrix[2][2] = p_zf / (p_zf - p_zn);
    m_matrix[3][2] = -p_zn * m_matrix[2][2];
    m_matrix[2][3] = 1.0f;

    m_configinfos.type = ConfigurationType::CONFIG_UNDETERMINED;
}


void Matrix::transpose(void)
{
    dsreal msave[4][4];

    memcpy(&msave, m_matrix, 16 * sizeof(dsreal));

    m_matrix[0][1] = msave[1][0];
    m_matrix[0][2] = msave[2][0];
    m_matrix[0][3] = msave[3][0];

    m_matrix[1][0] = msave[0][1];

    m_matrix[1][2] = msave[2][1];
    m_matrix[1][3] = msave[3][1];

    m_matrix[2][0] = msave[0][2];
    m_matrix[2][1] = msave[1][2];

    m_matrix[2][3] = msave[3][2];

    m_matrix[3][0] = msave[0][3];
    m_matrix[3][1] = msave[1][3];
    m_matrix[3][2] = msave[2][3];

    m_configinfos.type = ConfigurationType::CONFIG_UNDETERMINED;
}

void Matrix::scale(dsreal p_sx, dsreal p_sy, dsreal p_sz)
{
    zero();
    m_matrix[0][0] = p_sx;
    m_matrix[1][1] = p_sy;
    m_matrix[2][2] = p_sz;
    m_matrix[3][3] = 1.0;

    m_configinfos.type = ConfigurationType::CONFIG_SCALING;
    m_configinfos.values[0] = p_sx;
    m_configinfos.values[1] = p_sy;
    m_configinfos.values[2] = p_sz;
    m_configinfos.values[3] = 0.0;

}

void Matrix::scale(const Maths::Vector& p_pos)
{
    zero();
    m_matrix[0][0] = p_pos[0];
    m_matrix[1][1] = p_pos[1];
    m_matrix[2][2] = p_pos[2];
    m_matrix[3][3] = 1.0;

    m_configinfos.type = ConfigurationType::CONFIG_SCALING;
    m_configinfos.values[0] = p_pos[0];
    m_configinfos.values[1] = p_pos[1];
    m_configinfos.values[2] = p_pos[2];
    m_configinfos.values[3] = 0.0;

}


void Matrix::clearTranslation(void)
{
    m_matrix[3][0] = 0.0;
    m_matrix[3][1] = 0.0;
    m_matrix[3][2] = 0.0;

    m_configinfos.type = ConfigurationType::CONFIG_UNDETERMINED;
}

void Matrix::inverse( void )
{
    Matrix t;
    Matrix res;

    t.translation( -m_matrix[3][0], -m_matrix[3][1], -m_matrix[3][2] );

    clearTranslation();
    transpose();
    matrixMult( &t, this, &res );

    memcpy( &m_matrix, &res.m_matrix, 16 * sizeof( dsreal ) );
}

void Matrix::rotation( const Maths::Vector& p_axis, dsreal p_angle )
{
    const auto c{ std::cos(p_angle) };
    const auto s{ std::sin(p_angle) };
    const auto omc{ (1 - c) };

    auto axis{ p_axis };

    axis.normalize();

    const dsreal x{ axis[0] };
    const dsreal y{ axis[1] };
    const dsreal z{ axis[2] };
    const dsreal xs{ x * s };
    const dsreal ys{ y * s };
    const dsreal zs{ z * s };
    const dsreal xyomc { x * y * omc };
    const dsreal xzomc { x * z * omc };
    const dsreal yzomc { y * z * omc };

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

    m_configinfos.type = ConfigurationType::CONFIG_ROTATION;

    m_configinfos.values = p_axis;
    m_configinfos.values[3] = p_angle;
}

void Matrix::transform(Maths::Vector* p_vec_in, Maths::Vector* p_vec_out ) const
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


void Matrix::matrixMult( Matrix* p_mA, Matrix* p_mB, Matrix* p_mRes )
{
    for( int i = 0; i < 4; i++)
    {
        for( int j = 0; j < 4; j++ )
        {
            p_mRes->m_matrix[i][j] = p_mA->m_matrix[i][0] * p_mB->m_matrix[0][j] +
                                     p_mA->m_matrix[i][1] * p_mB->m_matrix[1][j] +
                                     p_mA->m_matrix[i][2] * p_mB->m_matrix[2][j] +
                                     p_mA->m_matrix[i][3] * p_mB->m_matrix[3][j];
        }
    }

    p_mRes->m_configinfos.type = ConfigurationType::CONFIG_UNDETERMINED;
}


Matrix operator* ( Matrix p_mA, Matrix p_mB )
{
    Matrix res;

    for( int i = 0; i < 4; i++)
    {
        for( int j = 0; j < 4; j++ )
        {
            res( i, j ) = p_mA( i, 0 ) * p_mB( 0, j ) +
                          p_mA( i, 1 ) * p_mB( 1, j ) +
                          p_mA( i, 2 ) * p_mB( 2, j ) +
                          p_mA( i, 3 ) * p_mB( 3, j );
        }
    }
    return res;
}




