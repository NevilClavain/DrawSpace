
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

    
    Output.t0.xyz = v_position2.xyz * 10.25;
    Output.t0.w = 1.0;

    //float3 v3CameraPos = 10.0 * (viewer_pos / flag0.z);

    float3 vc;
    vc.x = matCam[3][0];
    vc.y = matCam[3][1];
    vc.z = matCam[3][2];

    float3 v3CameraPos = 10.0 * (vc / flag0.z);
    Output.t1.xyz = v3CameraPos;
    Output.t1.w = 1.0;
    



    /*
    ////////////////////////////////////////////////////////////

    float3 ldir;
    ldir.x = -1.0;
    ldir.y = 0.0;
    ldir.z = 0.0;

    float3 v3CameraPos = 10.0 * ( viewer_pos / flag0.z );

    ////////////////////////////////////////////////////

    // The number of sample points taken along the ray
    int nSamples = 2;
    float fSamples = (float) nSamples;

    float fScaleDepth = 0.25;
    float fInvScaleDepth = 1.0 / fScaleDepth;


    /////////////////////////////////////////////////////

    float fOuterRadius = 10.25;
     //6100.0 * 1000.0; //* atmo_scattering_params.x;
    float fInnerRadius = 10.0;
     //6000.0 * 1000.0; //* atmo_scattering_params.y;

    float fOuterRadius2 = fOuterRadius * fOuterRadius;
    float fInnerRadius2 = fInnerRadius * fInnerRadius;

    float fCameraHeight2 = length(v3CameraPos) * length(v3CameraPos);


    float fScale = 1.0 / (fOuterRadius - fInnerRadius);

    float fScaleOverScaleDepth = fScale / fScaleDepth;

    float3 v3InvWavelength;

    v3InvWavelength.x = 1.0 / pow(0.650, 4.0);
    v3InvWavelength.y = 1.0 / pow(0.570, 4.0);
    v3InvWavelength.z = 1.0 / pow(0.475, 4.0);

    float fKr = 0.0025;
    float fKm = 0.0010;

    float fKr4PI = fKr * 4.0 * 3.1415927;

    float fKm4PI = fKm * 4.0 * 3.1415927;

    float ESun = 10.0;

    float fKrESun = fKr * ESun;
    float fKmESun = fKm * ESun;
	  
    //////////////////////////

    float3 v3Pos = v_position2.xyz * 10.25;
    float3 v3Ray = v3Pos - (v3CameraPos);
    float fFar = length(v3Ray);
    v3Ray /= fFar;


	// Calculate the closest intersection of the ray with the outer atmosphere (which is the near point of the ray passing through the atmosphere)
    float fNear = getNearIntersection(v3CameraPos, v3Ray, fCameraHeight2, fOuterRadius2);

	// Calculate the ray's start and end positions in the atmosphere, then calculate its scattering offset
    float3 v3Start = (v3CameraPos) + v3Ray * fNear;
    fFar -= fNear;
    float fStartAngle = dot(v3Ray, v3Start) / fOuterRadius;
    float fStartDepth = exp(-fInvScaleDepth);
    float fStartOffset = fStartDepth * scale(fStartAngle);

	// Initialize the scattering loop variables
    float fSampleLength = fFar / fSamples;
    float fScaledLength = fSampleLength * fScale;
    float3 v3SampleRay = v3Ray * fSampleLength;
    float3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
    float3 v3FrontColor = float3(0.0, 0.0, 0.0);
    for (int i = 0; i < nSamples; i++)
    {
        float fHeight = length(v3SamplePoint);
        float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
        float fLightAngle = dot(ldir, v3SamplePoint) / fHeight;
        float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;

        float fScatter = (fStartOffset + fDepth * (scale(fLightAngle) - scale(fCameraAngle)));
        float3 v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
        v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
        v3SamplePoint += v3SampleRay;
    }

	// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader

    Output.c0.w = 0.0;
    Output.c1.w = 0.0;
    Output.t0.w = 0.0;

    Output.c0.xyz = v3FrontColor * (v3InvWavelength * fKrESun);
    Output.c1.xyz = v3FrontColor * fKmESun;
    Output.t0.xyz = (v3CameraPos) - v3Pos;

*/

	return( Output );   
}
