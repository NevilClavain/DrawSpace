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

#ifndef _D3D9VERTEX_H_
#define _D3D9VERTEX_H_

typedef struct
{
    float   x;
    float   y;
    float   z;

    float   nx;
    float   ny;
    float   nz;

    float   tu0;
    float   tv0;
    float   tw0;
    float   ta0;

    float   tu1;
    float   tv1;
    float   tw1;
    float   ta1;

    float   tu2;
    float   tv2;
    float   tw2;
    float   ta2;

    float   tu3;
    float   tv3;
    float   tw3;
    float   ta3;

    float   tu4;
    float   tv4;
    float   tw4;
    float   ta4;

    float   tu5;
    float   tv5;
    float   tw5;
    float   ta5;

    float   tu6;
    float   tv6;
    float   tw6;
    float   ta6;

    float   tu7;
    float   tv7;
    float   tw7;
    float   ta7;

    float   tu8;
    float   tv8;
    float   tw8;
    float   ta8;

    /*
    float   tu9;
    float   tv9;
    float   tw9;
    float   ta9;
    */

} d3d9vertex;

#endif
