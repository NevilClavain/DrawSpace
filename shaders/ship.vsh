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


float4x4 matWorldViewProjection: register(c0);
float4x4 matWorldView : register(c4);
float4x4 matWorld : register(c8);
float4x4 matView : register(c12);
float4x4 matCam : register(c16);
float4x4 matProj : register(c20);

float4 mirror_flag : register(c25);
    // .x -> mirror mode
    // .y -> planet ground ray

float4 viewer_pos : register(c26);
float4 planetPos : register(c27);

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float4 TexCoord0: TEXCOORD0;
      
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float4 TexCoord0: TEXCOORD0;
};


#include "landscapes.hlsl"

VS_OUTPUT vs_main( VS_INPUT Input )
{
    VS_OUTPUT Output;

    //Output.Position = mul( Input.Position, matWorldViewProjection );

    if (mirror_flag.x > 0.0)
    {
        // calculer normale et position du plan de reflection
        float4 rn = normalize(viewer_pos);
        float4 rp = rn * mirror_flag.y;
        rp += planetPos;

        Output.Position = reflected_vertex_pos(Input.Position, rp, rn, matWorld, matView, matProj);
    }
    else
    {

        Output.Position = mul(Input.Position, matWorldViewProjection);
    }
    
    Output.TexCoord0 = Input.TexCoord0;
      
    return( Output );   
}
