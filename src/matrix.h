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

#pragma once

#include <cmath>
#include "vector.h"

namespace DrawSpace
{
namespace Utils
{
class Matrix
{
public:

    enum class ConfigurationType
    {
        CONFIG_UNDETERMINED,
        CONFIG_ZERO,
        CONFIG_IDENTITY,
        CONFIG_TRANSLATION,
        CONFIG_SCALING,
        CONFIG_ROTATION

    };

    typedef struct
    {
        ConfigurationType   type;
        Vector              values;

    } ConfigurationInfo;

protected:

    ConfigurationInfo   m_configinfos;
    dsreal              m_matrix[4][4];

public:

    Matrix( void );
    ~Matrix( void );

    void GetConfigInfos( ConfigurationInfo& p_infos )
    {
        p_infos = m_configinfos;
    }

    void Zero( void )
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

    dsreal operator()( int p_row, int p_col ) const
    {
        return m_matrix[p_row][p_col];
    };

    dsreal& operator()( int p_row, int p_col )
    {
        m_configinfos.type = ConfigurationType::CONFIG_UNDETERMINED;
        return m_matrix[p_row][p_col];
    };

    void Identity( void )
    {
        Zero();
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

    void Translation( dsreal p_x, dsreal p_y, dsreal p_z )
    {
        Identity();
        m_matrix[3][0] = p_x;
        m_matrix[3][1] = p_y;
        m_matrix[3][2] = p_z;

        m_configinfos.type = ConfigurationType::CONFIG_TRANSLATION;
        m_configinfos.values[0] = p_x;
        m_configinfos.values[1] = p_y;
        m_configinfos.values[2] = p_z;
        m_configinfos.values[3] = 0.0;

    }

    void Translation( const Vector& p_pos )
    {
        Identity();
        m_matrix[3][0] = p_pos[0];
        m_matrix[3][1] = p_pos[1];
        m_matrix[3][2] = p_pos[2];

        m_configinfos.type = ConfigurationType::CONFIG_TRANSLATION;
        m_configinfos.values[0] = p_pos[0];
        m_configinfos.values[1] = p_pos[1];
        m_configinfos.values[2] = p_pos[2];
        m_configinfos.values[3] = 0.0;

    }

    void Transpose( void )
    {
        dsreal msave[4][4];

        memcpy( &msave, m_matrix, 16 * sizeof( dsreal ) );

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

    void Perspective( dsreal p_w, dsreal p_h, dsreal p_zn, dsreal p_zf )
    {
        Zero();
        m_matrix[0][0] = 2.0f * p_zn / p_w;
        m_matrix[1][1] = 2.0f * p_zn / p_h;
        m_matrix[2][2] = p_zf / ( p_zf - p_zn );
        m_matrix[3][2] = - p_zn * m_matrix[2][2];
        m_matrix[2][3] = 1.0f;

        m_configinfos.type = ConfigurationType::CONFIG_UNDETERMINED;
    }

    void Scale( dsreal p_sx, dsreal p_sy, dsreal p_sz )
    {
        Zero();
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

    void Scale( const Vector& p_pos )
    {
        Zero();
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


    void ClearTranslation( void )
    {
        m_matrix[3][0] = 0.0;
        m_matrix[3][1] = 0.0;
        m_matrix[3][2] = 0.0;

        m_configinfos.type = ConfigurationType::CONFIG_UNDETERMINED;
    }

    void Rotation( const Vector& p_axis, dsreal p_angle );
    void Inverse( void );
    void Transform( Vector* p_vec_in, Vector* p_vec_out ) const;
    inline dsreal* GetArray( void ) const { return (dsreal*)m_matrix; };

    static void MatrixMult( Matrix* p_mA, Matrix* p_mB, Matrix* p_mRes );
};


}
}

DrawSpace::Utils::Matrix operator* ( DrawSpace::Utils::Matrix p_mA, DrawSpace::Utils::Matrix p_mB );


