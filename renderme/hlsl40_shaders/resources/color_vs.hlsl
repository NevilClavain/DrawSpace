/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

cbuffer constargs : register(b0)
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

    Matrix myMat;

    /*
    myMat[0][0] = 2.0f;
    myMat[0][1] = 0.0f;
    myMat[0][2] = 0.0f;
    myMat[0][3] = 0.0f;

    myMat[1][0] = 0.0f;
    myMat[1][1] = 3.111111111f;
    myMat[1][2] = 0.0f;
    myMat[1][3] = 0.0f;

    myMat[2][0] = 0.0f;
    myMat[2][1] = 0.0f;
    myMat[2][2] = -1.00001f;
    myMat[2][3] = 14.00014f;

    myMat[3][0] = 0.0f;
    myMat[3][1] = 0.0f;
    myMat[3][2] = -1.0f;
    myMat[3][3] = 15.0f;
    */

    
    myMat[0][0] = 2.0f;
    myMat[1][0] = 0.0f;
    myMat[2][0] = 0.0f;
    myMat[3][0] = 0.0f;

    myMat[0][1] = 0.0f;
    myMat[1][1] = 3.111111111f;
    myMat[2][1] = 0.0f;
    myMat[3][1] = 0.0f;

    myMat[0][2] = 0.0f;
    myMat[1][2] = 0.0f;
    myMat[2][2] = -1.00001f;
    myMat[3][2] = 14.00014f;

    myMat[0][3] = 0.0f;
    myMat[1][3] = 0.0f;
    myMat[2][3] = -1.0f;
    myMat[3][3] = 15.0f;
    

    //Output.Position = mul(pos, mat[matWorldViewProjection]);
    Output.Position = mul(pos, myMat);
      
    return( Output );   
}
