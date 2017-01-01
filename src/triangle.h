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

#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Core
{

class Triangle
{
public:
    long vertex1;
    long vertex2;
    long vertex3;

    Triangle( void ) :
    vertex1( 0 ),
    vertex2( 0 ),
    vertex3( 0 )
    {

    }

    Triangle( long p_i1, long p_i2, long p_i3 ) :
    vertex1( p_i1 ),
    vertex2( p_i2 ),
    vertex3( p_i3 )
    {

    }
};
}
}
#endif
