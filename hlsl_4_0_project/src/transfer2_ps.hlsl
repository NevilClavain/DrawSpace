/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

cbuffer legacyargs : register(b0)
{
    float4 vec[512];
    Matrix mat[512];
};

Texture2D txBackground         : register(t0);
SamplerState sam            : register(s0);


struct PS_INTPUT 
{
    float4 Position : SV_POSITION;
	float2 TexCoord0: TEXCOORD0;
};

#include "mat_input_constants.hlsl"
#include "fbm.hlsl"


//Y-axis rotation matrix
float3 rotateY(float3 p, float t)
{
	float cosTheta = cos(t);
	float sinTheta = sin(t);
	float3x3 rot = { cosTheta, 0.0, sinTheta,
				0.0, 1.0, 0.0,
				-sinTheta, 0.0, cosTheta };

	return mul( rot, p );
}

bool raycastSphere(float3 ro, float3 rd, out float3 p0, out float3 p1, float3 center, float r)
{
	// ajoute par moi : si la sphere est derriere la camera et que la camera est en dehors de la sphere 
	float r2 = dot(r, r);
	float d2 = dot(ro - center, ro - center);	
	if (d2 > r2 && dot(center-ro, rd) < 0.0)
	{
		return false;
	}

	////////////////////////////////////
	float A = dot(rd, rd);
	float B = 2.0 * (rd.x * (ro.x - center.x) + rd.y * (ro.y - center.y) + rd.z * (ro.z - center.z));
	float C = dot(ro - center, ro - center) - (r * r);

	float D = B * B - 4.0 * A * C;
	if (D < 0.0)
	{
		return false;
	}
	else
	{
		float t0 = (-B - D) / (2.0 * A);
		float t1 = (-B + D) / (2.0 * A);
		p0 = ro + rd * t0;
		p1 = ro + rd * t1;
		return true;
	}
}

#define MAX_STEPS 128
#define STEP_SIZE 0.05
#define SPHERE_RAY 1.33


float SpiralNoiseC(float3 p)
{
	float nudge = 0.739513;	// size of perpendicular vector
	float normalizer = 1.0 / sqrt(1.0 + nudge * nudge);	// pythagorean theorem on that perpendicular to maintain scale

	float n = 0.0;	// noise amount
	float iter = 1.0;
	for (int i = 0; i < 6; i++)
	{
		// add sin and cos scaled inverse with the frequency
		n += -abs(sin(p.y * iter) + cos(p.x * iter)) / iter;	// abs for a ridged look
		// rotate by adding perpendicular and scaling down
		p.xy += float2(p.y, -p.x) * nudge;
		p.xy *= normalizer;
		// rotate on other axis
		p.xz += float2(p.z, -p.x) * nudge;
		p.xz *= normalizer;
		// increase the frequency
		iter *= 1.733733;
	}
	return n;
}

//Cornette-Shanks phase function
float phase(float mu, float g)
{
	float g2 = g * g;
	return (3.0 * (1.0 - g2) * (1.0 + mu * mu)) / (2.0 * (2.0 + g2) * pow(1.0 + g2 - 2.0 * g * mu, 1.5));
}

//Density distribution function
float sampleVolume(float3 pos, float seed1, float seed2, bool rotate, float density)
{
	float rr = dot(pos, pos);
	rr = sqrt(rr);
	float f = exp(-rr);
	float p = f * density;

	if (p <= 0.0)
	{
		return p;
	}		
	p += SpiralNoiseC(seed1 + pos * 8.0) * 0.75;

	if(rotate) pos = rotateY(pos, pos.y * SpiralNoiseC(pos * 4.0) * 2.0);

	p += SpiralNoiseC(seed2 + pos * 3.0) * 1.5;
	p *= rr / SPHERE_RAY;

	p = max(0.0, p);

	return p;
}

struct LightParams
{
	float	_LightIntensity;
	float3  _Emission;
	float3	_Absorption;
	float   _Density;
};


struct NebParams
{
	int			nbExtraSpheres;
	
	LightParams lightParams[4];

	bool		sphereRotatey[4];
	
	float       seed1[4];
	float       seed2[4];

	float3		 spherePos[3];  // pos pour spheres 1 a 3
	
};

/////////////////////////////////////////////////////////////////////////////////

// vec[base] => emission color

// vec[base + 1] => absorption color

// vec[base + 2] => .x = number of extra sphere [1-3]
//				 => .y = sphere0 light intensity	

// vec[base + 3] => 4 spheres density [8.0 - 14.0]

// vec[base + 4] => 4 spheres rotatey twist [false - true]

// vec[base + 5] => seed1 for 4 spheres

// vec[base + 6] => seed2 for 4 spheres

// vec[base + 7] => sphere1 local pos
// vec[base + 8] => sphere2 local pos
// vec[base + 9] => sphere3 local pos


NebParams FillNebParams(int offset)
{
	NebParams nb;

	nb.nbExtraSpheres = vec[offset + 2].x;

	for (int i = 0; i < nb.nbExtraSpheres; i++)
	{
		
		nb.lightParams[i]._Emission = vec[offset].xyz;
		nb.lightParams[i]._Absorption = vec[offset + 1].xyz;

		if (i == 0)
		{
			// _LightIntensity pour la sphere 0 (sphere centrale) seulement

			nb.lightParams[0]._LightIntensity = vec[offset + 2].y;
			nb.lightParams[0]._Density = vec[offset + 3].x;

			nb.sphereRotatey[0] = vec[offset + 4].x;

			nb.seed1[0] = vec[offset + 5].x;
			nb.seed2[0] = vec[offset + 6].x;

			//nb.spherePos[0] = 0.0;
			
		}
		else if(i == 1)
		{
			nb.lightParams[1]._LightIntensity = 0.0;
			nb.lightParams[1]._Density = vec[offset + 3].y;

			nb.sphereRotatey[1] = vec[offset + 4].y;

			nb.seed1[1] = vec[offset + 5].y;
			nb.seed2[1] = vec[offset + 6].y;

			nb.spherePos[0] = vec[offset + 7];
		}
		else if (i == 2)
		{
			nb.lightParams[2]._LightIntensity = 0.0;
			nb.lightParams[2]._Density = vec[offset + 3].z;

			nb.sphereRotatey[2] = vec[offset + 4].z;

			nb.seed1[2] = vec[offset + 5].z;
			nb.seed2[2] = vec[offset + 6].z;

			nb.spherePos[1] = vec[offset + 8];
		}
		else
		{
			nb.lightParams[3]._LightIntensity = 0.0;
			nb.lightParams[3]._Density = vec[offset + 3].w;

			nb.sphereRotatey[3] = vec[offset + 4].w;

			nb.seed1[3] = vec[offset + 5].w;
			nb.seed2[3] = vec[offset + 6].w;

			nb.spherePos[2] = vec[offset + 9];
		}
		
	}

	return nb;
}

float4 raymarch(float3 pos, float3 dir, float ds, int s, float seed1, float seed2, bool rotate, LightParams lightParams)
{
	float3	_Light = float3(1.0, 1.0, 1.0);
	float3	_LightPos = float3(0.0, 0.0, 0.0);

	float	_Mie = 0.6;
	float	_G = 0.2;
	
	float4 result = float4(0.0, 0.0, 0.0, 1.0);

	int nb_steps = min(s, MAX_STEPS);
	for (int i = 0; i < nb_steps; i++)
	{
		float p = sampleVolume(pos, seed1, seed2, rotate, lightParams._Density);
		if (p > 0.0)
		{			
			float3 r = _LightPos - pos;
			float atten = lightParams._LightIntensity / dot(r, r);
			float3 ext = max(float3(0.000001, 0.000001, 0.000001), (lightParams._Absorption * p) + float3(_Mie * p, _Mie * p, _Mie * p));
			float3 trans = exp(-ext * ds);
			float3 lightDir = normalize(r);
			float mu = dot(lightDir, dir);
			float phase_res = phase(mu, _G);

			float3 lum = lightParams._Emission + _Light * phase_res * (1.0 - lightParams._Absorption) * _Mie * p * atten;
			float3 integral = (lum - (lum * trans)) / ext;

			result.rgb += integral * result.a;
			float3 div = float3(0.3333333, 0.3333333, 0.3333333);

			result.a *= dot(trans, div);

			if (result.a <= 0.1)
			{
				return result;
			}					
		}
		pos += dir * ds;
	}

	result.rgb = clamp(result.rgb, 0, 1);
	
	return result;
}

float3 computePixelColor(float3 current_color, float3 rayOrigin, float4 rayDir, float3 p0, float3 p1, float3 sphere_localpos, 
							float2 resolution, float2 texture_coords, float seed1, float seed2, bool rotate, LightParams lightParams)
{
	Matrix bayer = 
	{ 
		0.0, 0.5, 0.125, 0.625 ,
		0.75, 0.25, 0.875, 0.375,
		0.1875, 0.6875, 0.0625, 0.5625,
		0.9375, 0.4375, 0.8125, 0.3125
	};

	float3 updated_color;

	if (dot(rayOrigin, rayOrigin) < dot(p0, p0))
	{
		//camera a l'interieur de la sphere
		p0 = rayOrigin;
	}

	///////////////////////////////////////////////////////////

	//Bayer matrix ordered depth jittering
	
	float width = (texture_coords.x * resolution.x);
	float height = (texture_coords.y * resolution.y);
	width = fmod(width, 4.0);
	height = fmod(height, 4.0);
	float offset = bayer[int(width)][int(height)];
	p0 -= rayDir * offset * STEP_SIZE * 2.0;
	
	///////////////////////////////////////////////////////////

	p0 -= sphere_localpos;

	float dist = length(p1 - p0);
	int s = int(dist / STEP_SIZE) + 1;

	float4 integral = raymarch(p0, rayDir, STEP_SIZE, s, seed1, seed2, rotate, lightParams);
	
	updated_color = lerp(integral.rgb, current_color.rgb, integral.a);
	return updated_color;
}

float4 ps_main(PS_INTPUT input) : SV_Target
{
	float4 current_color;

	float4 camera_params = vec[0];
	Matrix mat_cam = mat[1];
	Matrix mat_pos = mat[5];

	float2 resolution = vec[10].xy;


	/////////////////////////////////////////////////////////////////////////////////

	float camera_width = camera_params.x;
	float camera_height = camera_params.y;
	float camera_zn = camera_params.z;

	float3 ray_origin;

	
	Matrix pos_rot = mat_pos;
	pos_rot[3][0] = 0.0;
	pos_rot[3][1] = 0.0;
	pos_rot[3][2] = 0.0;
	pos_rot = transpose(pos_rot); // inverse rot mat


	ray_origin.x = mat_cam[3][0];
	ray_origin.y = mat_cam[3][1];
	ray_origin.z = mat_cam[3][2];


	float3 container_pos;
	container_pos.x = mat_pos[3][0];
	container_pos.y = mat_pos[3][1];
	container_pos.z = mat_pos[3][2];

	float3 globalcontainer_rayOrigin = ray_origin - container_pos;
	float3 localcontainer_rayOrigin = mul(globalcontainer_rayOrigin, pos_rot);

	// compute camera-local ray direction
	float4 localcam_rayDir;
	localcam_rayDir.x = (input.TexCoord0.x - 0.5) * camera_width;
	localcam_rayDir.y = ((1.0 - input.TexCoord0.y) - 0.5) * camera_height;
	localcam_rayDir.z = -camera_zn;
	localcam_rayDir.w = 1.0;

	// compute global ray direction
	mat_cam[3][0] = 0.0;
	mat_cam[3][1] = 0.0;
	mat_cam[3][2] = 0.0;
	float4 global_rayDir = mul(localcam_rayDir, mat_cam);

	// compute local container ray direction
	float4 local_rayDir = mul(global_rayDir, pos_rot);
		
	current_color.rgb = txBackground.Sample(sam, input.TexCoord0);

	float3 p0;
	float3 p1;

	float3 sphere0_localpos = { 0.0, 0.0, 0.0 };

	NebParams neb0 = FillNebParams(11);

	// render central sphere (sphere0)
	if (raycastSphere(localcontainer_rayOrigin, local_rayDir.xyz, p0, p1, sphere0_localpos, SPHERE_RAY) == true)
	{
		current_color.rgb = computePixelColor(current_color.rgb, localcontainer_rayOrigin, local_rayDir, p0, p1, sphere0_localpos, resolution, input.TexCoord0, neb0.seed1[0], neb0.seed2[0], neb0.sphereRotatey[0], neb0.lightParams[0]);
	}

	for (int i = 0; i < neb0.nbExtraSpheres; i++)
	{
		if (raycastSphere(localcontainer_rayOrigin, local_rayDir.xyz, p0, p1, neb0.spherePos[i], SPHERE_RAY) == true)
		{
			current_color.rgb = computePixelColor(current_color.rgb, localcontainer_rayOrigin, local_rayDir, p0, p1, neb0.spherePos[i], resolution, input.TexCoord0, neb0.seed1[i+1], neb0.seed2[i+1], neb0.sphereRotatey[i+1], neb0.lightParams[i+1]);
		}
	}

	current_color.a = 1.0; //alpha value : whatever
	return current_color;
}
