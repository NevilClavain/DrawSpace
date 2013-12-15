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

#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Core
{
class Vertex
{
private:
    void init_text_coords( void )
    {
        for( long i = 0; i < /*10*/9; i++ )
        {
            tu[i] = 0.0;
            tv[i] = 0.0;
            tw[i] = 0.0;
            ta[i] = 0.0;
        }
    }
public:
    dsreal	x;
    dsreal	y;
    dsreal	z;

    dsreal	nx;
    dsreal	ny;
    dsreal	nz;

    /*
    float	tu[10];
    float	tv[10];
    float	tw[10];
    float	ta[10];
    */

    float	tu[9];
    float	tv[9];
    float	tw[9];
    float	ta[9];

    Vertex( void ):
    x( 0.0 ),
    y( 0.0 ),
    z( 0.0 ),
    nx( 0.0 ),
    ny( 0.0 ),
    nz( 0.0 )
    {
        init_text_coords();
    };

    Vertex( dsreal p_x, dsreal p_y, dsreal p_z ) :
    x( p_x ),
    y( p_y ),
    z( p_z )
    {
        init_text_coords();
    }

};
}
}
#endif
