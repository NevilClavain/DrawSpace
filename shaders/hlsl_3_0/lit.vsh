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


float4x4 matWorldViewProjection     : register(c0);
float4x4 matWorld                   : register(c8);
float4x4 matCam                     : register(c16);
float4 Lights_Enabled               : register(c24);
float4 Light0_Dir                   : register(c25);

struct VS_INPUT 
{
   float4 Position      : POSITION0;
   float3 Normal        : NORMAL0;
   float4 TexCoord0     : TEXCOORD0;    
};

struct VS_OUTPUT 
{
   float4 Position      : POSITION0;
   float4 TexCoord0     : TEXCOORD0;
   float4 Normale       : TEXCOORD1;
   float4 Half0         : TEXCOORD2;
};

#include "generic_rendering.hlsl"

VS_OUTPUT vs_main( VS_INPUT Input )
{
    VS_OUTPUT Output;

    Output.Position = mul( Input.Position, matWorldViewProjection );
    Output.TexCoord0 = Input.TexCoord0;

    Output.Normale = TransformedNormaleForLights(Input.Normal, matWorld);

    if (Lights_Enabled.x > 0.0)
    {
        Output.Half0 = HalfVectorForLights(Input.Position, Light0_Dir, matCam, matWorld);
    }
    else
    {        
        Output.Half0 = 0.0;
    }

    return( Output );   
}
