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
#include "ds_types.h"

namespace DrawSpace
{
    namespace Maths
    {
        class Vector
        {
        public:

            Vector( void );
            Vector( dsreal p_x, dsreal p_y, dsreal p_z, dsreal p_w );
            Vector( dsreal p_x, dsreal p_y );

            ~Vector(void) = default;

            dsreal operator[]( int p_index ) const
            {
                return m_vector[p_index];
            };

            dsreal& operator[]( int p_index )
            {
                return m_vector[p_index];
            };

            dsreal lengthPow2( void ) const;	
            dsreal length( void ) const;

            void normalize( void );
            void scale( dsreal p_scale );

	        static Vector lerp(const Vector& p_v1, const Vector& p_v2, dsreal p_blend);

            static Vector prodVec(const DrawSpace::Maths::Vector& p_vA, const DrawSpace::Maths::Vector& p_vB);

        private:
            dsreal m_vector[4];

        };
    }
}

dsreal operator* ( const DrawSpace::Maths::Vector& p_vA, const DrawSpace::Maths::Vector& p_vB );
DrawSpace::Maths::Vector operator+ ( const DrawSpace::Maths::Vector& p_vA, const DrawSpace::Maths::Vector& p_vB );



