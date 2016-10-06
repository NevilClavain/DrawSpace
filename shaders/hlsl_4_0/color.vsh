
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
