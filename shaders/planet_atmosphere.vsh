
float4x4 matWorldViewProjection: register(c0);
float4x4 matWorldView: register(c4);


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


float4 atmo_scattering_params: register(c30);
	// .x -> OuterRadius
	// .y -> InnerRadius
	// .z -> light power
    // .w -> optical lookup table side size



sampler2D TextureLookupTable : register(s0);

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float4 TexCoord0: TEXCOORD0; 
};

struct VS_OUTPUT 
{
   float4 Position                  : POSITION0;
   float4 LODGlobalPatch_TexCoord	: TEXCOORD0;
   float4 UnitPatch_TexCoord		: TEXCOORD1;
   float4 GlobalPatch_TexCoord		: TEXCOORD2;
   float4 color                     : TEXCOORD3;
};

#include "fbm.hlsl"
#include "multifbm_height.hlsl"
#include "spherelod_commons.hlsl"

float4 getLookupTableValue(int p_index_u, int p_index_v)
{
    float v_int_center = 1.0 / (2.0 * atmo_scattering_params.w);
    float v_int = 1.0 / atmo_scattering_params.w;

    float4 uv_l = 0.0;

    uv_l.y = (p_index_v * v_int) + v_int_center;
    uv_l.x = (p_index_u * v_int) + v_int_center;

    return tex2Dlod(TextureLookupTable, uv_l);
}

float4 interpolate( float p_x, float p_y )
{
    float4 res;
    float fX = p_x * (atmo_scattering_params.w - 1);
    float fY = p_y * (atmo_scattering_params.w - 1);

    int nX = min((int)atmo_scattering_params.w - 2, max(0, (int) fX));

    int nY = min((int)atmo_scattering_params.w - 2, max(0, (int) fY));
    float fRatioX = fX - nX;
    float fRatioY = fY - nY;

    float4 point1 = getLookupTableValue(nX, nY);
    float4 point2 = getLookupTableValue(nX + 1, nY);
    float4 point3 = getLookupTableValue(nX, nY + 1);
    float4 point4 = getLookupTableValue(nX + 1, nY + 1);

    res.x = point1.x * (1 - fRatioX) * (1 - fRatioY) +
				 point2.x * (fRatioX) * (1 - fRatioY) +
				 point3.x * (1 - fRatioX) * (fRatioY) +
				 point4.x * (fRatioX) * (fRatioY);

    res.y = point1.y * (1 - fRatioX) * (1 - fRatioY) +
				 point2.y * (fRatioX) * (1 - fRatioY) +
				 point3.y * (1 - fRatioX) * (fRatioY) +
				 point4.y * (fRatioX) * (fRatioY);

    res.z = point1.z * (1 - fRatioX) * (1 - fRatioY) +
				 point2.z * (fRatioX) * (1 - fRatioY) +
				 point3.z * (1 - fRatioX) * (fRatioY) +
				 point4.z * (fRatioX) * (fRatioY);

    res.w = point1.w * (1 - fRatioX) * (1 - fRatioY) +
				 point2.w * (fRatioX) * (1 - fRatioY) +
				 point3.w * (1 - fRatioX) * (fRatioY) +
				 point4.w * (fRatioX) * (fRatioY);

    return res;
}

float4 scattering_color( float4 p_vertex, float4 p_viewpos, float4 p_ldir )
{

}

VS_OUTPUT vs_main( VS_INPUT Input )
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
	
	Output.LODGlobalPatch_TexCoord = 0.0;
	Output.LODGlobalPatch_TexCoord.x = lerp( base_uv.x, base_uv.z, Input.TexCoord0.x );
	Output.LODGlobalPatch_TexCoord.y = lerp( base_uv.y, base_uv.w, Input.TexCoord0.y );

	// conserver aussi les coords textures originales du patch
	Output.UnitPatch_TexCoord = 0.0;
	Output.UnitPatch_TexCoord.x = Input.TexCoord0.x;
	Output.UnitPatch_TexCoord.y = Input.TexCoord0.y;

	
	Output.GlobalPatch_TexCoord = 0.0;
	Output.GlobalPatch_TexCoord.x = lerp( base_uv_global.x, base_uv_global.z, Input.TexCoord0.x );
	Output.GlobalPatch_TexCoord.y = lerp( base_uv_global.y, base_uv_global.w, Input.TexCoord0.y );

    float4 color = getLookupTableValue(50, 50);

    Output.color = color;
			  
	return( Output );   
}
