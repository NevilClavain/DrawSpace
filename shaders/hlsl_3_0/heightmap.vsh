/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

float4x4 matWorldViewProjection: register(c0);

sampler2D TextureHeightMap : register(s0);

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

VS_OUTPUT vs_main( VS_INPUT Input )
{
    VS_OUTPUT Output;

    float4 vertex = Input.Position;

    vertex.y = -2.0;

    float4 color_height = tex2Dlod(TextureHeightMap, Input.TexCoord0);

    vertex.y += 10.0 * color_height.r;

    Output.Position = mul( vertex, matWorldViewProjection );


    Output.TexCoord0 = Input.TexCoord0;
      
    return( Output );   
}
