
float4x4 matWorldViewProjection: register(c0);
float4x4 matWorldView: register(c4);
float4x4 matWorld : register(c8);
float4x4 matCam : register(c16);


float4   flag0:				register(c24);

	// .x -> patch orientation enum integer
	// .y -> patch sidelenght
	// .z -> planet ray
	// .w -> relative alt
	

float4   patch_translation:	register(c25);
	// .x, .y -> patch positionning

float4   base_uv: register(c26);
	// .x, .y -> u1, v1
	// .z, .w -> u2, v2

float4   base_uv_global: register(c27);
	// .x, .y -> u1, v1
	// .z, .w -> u2, v2

float4   viewer_pos : register(c28);


float4 atmo_scattering_params: register(c30);
	// .x -> OuterRadius
	// .y -> InnerRadius
	// .z -> light power
    // .w -> optical lookup table side size


struct VS_INPUT 
{
   float4 Position : POSITION0;
   float4 TexCoord0: TEXCOORD0; 
};

struct VS_OUTPUT 
{
   float4 Position                  : POSITION0;
    //float4 c0 : COLOR0;
    //float4 c1 : COLOR1;
   float4 t0		                : TEXCOORD0;
   float4 t1 : TEXCOORD1;

};

#include "fbm.hlsl"
#include "multifbm_height.hlsl"
#include "spherelod_commons.hlsl"

// Returns the near intersection point of a line and a sphere
float getNearIntersection(float3 v3Pos, float3 v3Ray, float fDistance2, float fRadius2)
{
    float B = 2.0 * dot(v3Pos, v3Ray);
    float C = fDistance2 - fRadius2;
    float fDet = max(0.0, B * B - 4.0 * C);
    return 0.5 * (-B - sqrt(fDet));
}

// The scale equation calculated by Vernier's Graphical Analysis
float scale(float fCos)
{
    float fScaleDepth = 0.25;
    float x = 1.0 - fCos;
    return fScaleDepth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

VS_OUTPUT vs_main(VS_INPUT Input)
{
	VS_OUTPUT Output;

	float4 v_position;

	// sidelenght scaling

	v_position = Input.Position * flag0.y / 2.0;
	v_position = v_position + patch_translation;
	v_position.z = 1.0;
	v_position.w = 1.0;
	
	float4 v_position2;	
	v_position2.w = 1.0;
	v_position2.xyz = CubeToSphere( ProjectVectorToCube( flag0.x, v_position.xyz ) );

	// final scaling
	float4 v_position3;	
	v_position3 = v_position2 * flag0.z;	
	v_position3.w = 1.0;


	Output.Position = mul( v_position3, matWorldViewProjection );


    ////// atmo scattering : calcul vertex pos

    float4x4 matWorldRot = matWorld;

    // clear translations matWorld
    matWorldRot[3][0] = 0.0;
    matWorldRot[3][1] = 0.0;
    matWorldRot[3][2] = 0.0;

    float4 vertex_pos = mul(v_position2, matWorldRot);

    // enfin, mise à l'échelle
    vertex_pos *= 10.25;

    Output.t0 = vertex_pos;

    Output.t1.xyz = 10.0 * (viewer_pos / flag0.z);
    Output.t1.w = 1.0;

	return( Output );   
}
