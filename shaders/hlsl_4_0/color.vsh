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


cbuffer legacyargs : register(b0)
{
    float4 vec[512];
    Matrix mat[512];
};

#include "mat_input_constants.hlsl"

struct VS_INPUT 
{
   float3 Position : POSITION;
};

struct VS_OUTPUT 
{
   float4 Position : SV_POSITION;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
    VS_OUTPUT Output;
    float4 pos;
    
    pos.xyz = Input.Position;    
    pos.w = 1.0;

    Output.Position = mul(pos, mat[matWorldViewProjection]);
      
    return( Output );   
}
