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

struct VS_INPUT 
{
   float4 Position  : POSITION0;
   float3 Normal    : NORMAL0;
   float4 TexCoord0 : TEXCOORD0;    
};

struct VS_OUTPUT 
{
   float4 Position  : POSITION0;
   float4 TexCoord0 : TEXCOORD0;
   float4 Normale : TEXCOORD1;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
    VS_OUTPUT Output;

    Output.Position = mul( Input.Position, matWorldViewProjection );
    Output.TexCoord0 = Input.TexCoord0;

	// NORMAL : pour lumieres diffuses : NORMAL transformee (sans les translations) dans repere world
    float4x4 worldRot = matWorld;
    worldRot[0][3] = 0.0;
    worldRot[1][3] = 0.0;
    worldRot[2][3] = 0.0;

    Output.Normale = mul(Input.Normal, worldRot);

    return( Output );   
}
