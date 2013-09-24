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

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cmath>
#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Utils
{
class Vector
{
protected:

    dsreal m_vector[4];

public:

    Vector( void );
    Vector( dsreal p_x, dsreal p_y, dsreal p_z, dsreal p_w );
    Vector( dsreal p_x, dsreal p_y );

    ~Vector( void );

    dsreal* GetArray( void );

    dsreal operator[]( int p_index ) const
    {
        return m_vector[p_index];
    };

    dsreal& operator[]( int p_index )
    {
        return m_vector[p_index];
    };

    dsreal LengthPow2( void );	
    dsreal Length( void );

    void Normalize( void );
    void Scale( dsreal p_scale );
};

dsreal operator* ( Vector p_vA, Vector p_vB );
Vector operator+ ( Vector p_vA, Vector p_vB );

Vector ProdVec( Vector p_vA, Vector p_vB );
}
}

#endif