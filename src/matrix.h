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
    namespace Maths
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

            struct ConfigurationInfo
            {
                ConfigurationType   type;
                Maths::Vector       values;
            };


            Matrix( void );
            ~Matrix( void ) = default;

            void zero(void);

            dsreal operator()( int p_row, int p_col ) const
            {
                return m_matrix[p_row][p_col];
            };

            dsreal& operator()( int p_row, int p_col )
            {
                m_configinfos.type = ConfigurationType::CONFIG_UNDETERMINED;
                return m_matrix[p_row][p_col];
            };

            void identity(void);
            void translation(dsreal p_x, dsreal p_y, dsreal p_z);
            void translation(const Maths::Vector& p_pos);
            void transpose(void);
            void perspective(dsreal p_w, dsreal p_h, dsreal p_zn, dsreal p_zf);

            void scale(dsreal p_sx, dsreal p_sy, dsreal p_sz);
            void scale(const Maths::Vector& p_pos);
            void clearTranslation(void);

            void rotation( const Maths::Vector& p_axis, dsreal p_angle );
            void inverse( void );
            void transform(Maths::Vector* p_vec_in, Maths::Vector* p_vec_out ) const;

            dsreal* getArray( void ) const 
            { 
                return (dsreal*)m_matrix; 
            };

            static void matrixMult( Matrix* p_mA, Matrix* p_mB, Matrix* p_mRes );

        private:

            ConfigurationInfo   m_configinfos;
            dsreal              m_matrix[4][4];

        };
    }
}

DrawSpace::Maths::Matrix operator* ( DrawSpace::Maths::Matrix p_mA, DrawSpace::Maths::Matrix p_mB );


