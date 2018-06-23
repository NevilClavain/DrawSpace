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

    float4 CamPos;
    float4 Pos2;
    float3 nView;
    float3 nLight;

    CamPos[0] = matCam[3][0];
    CamPos[1] = matCam[3][1];
    CamPos[2] = matCam[3][2];
    CamPos[3] = 1.0;

    Pos2 = mul(Input.Position, matWorld);
    nView = normalize(CamPos.xyz - Pos2.xyz);

    if (Lights_Enabled.x > 0.0)
    {
        nLight = normalize(-Light0_Dir.xyz);
        Output.Half0.xyz = (nLight + nView);
        Output.Half0[3] = 0.0;
    }
    else
    {
        Output.Half0 = 0.0;
    }

    return( Output );   
}
