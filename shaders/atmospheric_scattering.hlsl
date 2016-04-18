/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

// Calculates the Rayleigh phase function
float getRayleighPhase(float fCos2)
{
	//return 1.0;
    return 0.75 + 0.75 * fCos2;
}

// Calculates the Mie phase function
float getMiePhase(float fCos, float fCos2, float g, float g2)
{
    return 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos2) / pow(1.0 + g2 - 2.0 * g * fCos, 1.5);
}

struct atmo_scattering_sampling_result
{
    float3 c0;
    float3 c1;
    float3 v3Direction;
};

atmo_scattering_sampling_result skyfromspace_atmo_scattering_sampling(float3 p_vertex_pos, float3 p_camera_pos, float3 p_ldir)
{
    atmo_scattering_sampling_result res;

    float3 v3CameraPos = p_camera_pos;

    ////////////////////////////////////////////////////

    // The number of sample points taken along the ray
    int nSamples = 2;
    float fSamples = (float) nSamples;

    float fScaleDepth = 0.25;
    float fInvScaleDepth = 1.0 / fScaleDepth;


    /////////////////////////////////////////////////////

    //float fOuterRadius = 10.25;
    //float fInnerRadius = 10.0;

    float fOuterRadius = 6085000.0;
    float fInnerRadius = 6000000.0;


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

    float ESun = 3.0;
     //12.0;

    float fKrESun = fKr * ESun;
    float fKmESun = fKm * ESun;
	  
    //////////////////////////

    float3 v3Pos = p_vertex_pos;

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
        float fLightAngle = dot(p_ldir, v3SamplePoint) / fHeight;
        float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;

        float fScatter = (fStartOffset + fDepth * (scale(fLightAngle) - scale(fCameraAngle)));
        float3 v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
        v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
        v3SamplePoint += v3SampleRay;
    }

    // Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
    res.c0 = v3FrontColor * (v3InvWavelength * fKrESun);
    res.c1 = v3FrontColor * fKmESun;
    res.v3Direction = (v3CameraPos) - v3Pos;

    return res;
}

float3 atmo_scattering_color_result(atmo_scattering_sampling_result p_sampling, float3 p_ldir)
{
    float g = -0.990;
    float g2 = g * g;

    float3 color;

    float fCos = dot(p_ldir, p_sampling.v3Direction) / length(p_sampling.v3Direction);
    float fCos2 = fCos * fCos;
    color = getRayleighPhase(fCos2) * p_sampling.c0 + getMiePhase(fCos, fCos2, g, g2) * p_sampling.c1;

    return color;
}

atmo_scattering_sampling_result skyfromatmo_atmo_scattering_sampling(float3 p_vertex_pos, float3 p_camera_pos, float3 p_ldir)
{
    atmo_scattering_sampling_result res;

    float3 v3CameraPos = p_camera_pos;

    ////////////////////////////////////////////////////

    // The number of sample points taken along the ray
    int nSamples = 2;
    float fSamples = (float) nSamples;

    float fScaleDepth = 0.25;
    float fInvScaleDepth = 1.0 / fScaleDepth;


    /////////////////////////////////////////////////////

    //float fOuterRadius = 10.25;
    //float fInnerRadius = 10.0;

    float fOuterRadius = 6085000.0;
    float fInnerRadius = 6000000.0;

    float fOuterRadius2 = fOuterRadius * fOuterRadius;
    float fInnerRadius2 = fInnerRadius * fInnerRadius;

    float fCameraHeight = length(v3CameraPos);
    float fCameraHeight2 = fCameraHeight * fCameraHeight;


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

    float ESun = 12.0;

    float fKrESun = fKr * ESun;
    float fKmESun = fKm * ESun;
	  
    //////////////////////////

    float3 v3Pos = p_vertex_pos;

    float3 v3Ray = v3Pos - (v3CameraPos);
    float fFar = length(v3Ray);
    v3Ray /= fFar;

	// Calculate the ray's starting position, then calculate its scattering offset
    float3 v3Start = v3CameraPos;
    float fHeight = length(v3Start);
    float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
    float fStartAngle = dot(v3Ray, v3Start) / fHeight;
    float fStartOffset = fDepth * scale(fStartAngle);

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
        float fLightAngle = dot(p_ldir, v3SamplePoint) / fHeight;
        float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
        float fScatter = (fStartOffset + fDepth * (scale(fLightAngle) - scale(fCameraAngle)));
        float3 v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
        v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
        v3SamplePoint += v3SampleRay;
    }

	// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader

    res.c0 = v3FrontColor * (v3InvWavelength * fKrESun);
    res.c1 = v3FrontColor * fKmESun;
    res.v3Direction = v3CameraPos - v3Pos;

    return res;
}

atmo_scattering_sampling_result groundfromspace_atmo_scattering_sampling(float3 p_vertex_pos, float3 p_camera_pos, float3 p_ldir)
{
    atmo_scattering_sampling_result res;

    float3 v3CameraPos = p_camera_pos;

    ////////////////////////////////////////////////////

    // The number of sample points taken along the ray
    int nSamples = 2;
    float fSamples = (float) nSamples;

    float fScaleDepth = 0.25;
    float fInvScaleDepth = 1.0 / fScaleDepth;


    /////////////////////////////////////////////////////


    float fOuterRadius = 6085000.0;
    float fInnerRadius = 6000000.0;

    float fOuterRadius2 = fOuterRadius * fOuterRadius;
    float fInnerRadius2 = fInnerRadius * fInnerRadius;

    float fCameraHeight = length(v3CameraPos);
    float fCameraHeight2 = fCameraHeight * fCameraHeight;


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

    float ESun = 24.0; //12.0;

    float fKrESun = fKr * ESun;
    float fKmESun = fKm * ESun;
	  
    //////////////////////////
   
    float3 v3Pos = p_vertex_pos;

    float3 v3Ray = v3Pos - (v3CameraPos);
    v3Pos = normalize(v3Pos);
    float fFar = length(v3Ray);
    v3Ray /= fFar;

	// Calculate the closest intersection of the ray with the outer atmosphere (which is the near point of the ray passing through the atmosphere)
    float fNear = getNearIntersection(v3CameraPos, v3Ray, fCameraHeight2, fOuterRadius2);

	// Calculate the ray's starting position, then calculate its scattering offset
    float3 v3Start = v3CameraPos + v3Ray * fNear;
    fFar -= fNear;
    float fDepth = exp((fInnerRadius - fOuterRadius) * fInvScaleDepth);
    float fCameraAngle = dot(-v3Ray, v3Pos);
    float fLightAngle = dot(p_ldir, v3Pos);
    float fCameraScale = scale(fCameraAngle);
    float fLightScale = scale(fLightAngle);
    float fCameraOffset = fDepth * fCameraScale;
    float fTemp = (fLightScale + fCameraScale);

	// Initialize the scattering loop variables
    float fSampleLength = fFar / fSamples;
    float fScaledLength = fSampleLength * fScale;
    float3 v3SampleRay = v3Ray * fSampleLength;
    float3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
    float3 v3FrontColor = float3(0.0, 0.0, 0.0);
    float3 v3Attenuate;
    for (int i = 0; i < nSamples; i++)
    {
        float fHeight = length(v3SamplePoint);
        float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
        float fScatter = fDepth * fTemp - fCameraOffset;
        v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
        v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
        v3SamplePoint += v3SampleRay;
    }

    res.c0 = v3FrontColor * (v3InvWavelength * fKrESun + fKmESun);
    res.c1 = v3Attenuate;
    res.v3Direction = 0.0;
    return res;
}

atmo_scattering_sampling_result groundfromatmo_atmo_scattering_sampling(float3 p_vertex_pos, float3 p_camera_pos, float3 p_ldir)
{
    atmo_scattering_sampling_result res;

    float3 v3CameraPos = p_camera_pos;

    ////////////////////////////////////////////////////

    // The number of sample points taken along the ray
    int nSamples = 2;
    float fSamples = (float) nSamples;

    float fScaleDepth = 0.25;
    float fInvScaleDepth = 1.0 / fScaleDepth;


    /////////////////////////////////////////////////////


    float fOuterRadius = 6085000.0;
    float fInnerRadius = 6000000.0;

    float fOuterRadius2 = fOuterRadius * fOuterRadius;
    float fInnerRadius2 = fInnerRadius * fInnerRadius;

    float fCameraHeight = length(v3CameraPos);
    float fCameraHeight2 = fCameraHeight * fCameraHeight;


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
   
    float3 v3Pos = p_vertex_pos;

    float3 v3Ray = v3Pos - (v3CameraPos);
    v3Pos = normalize(v3Pos);
    float fFar = length(v3Ray);
    v3Ray /= fFar;

	// Calculate the ray's starting position, then calculate its scattering offset
    float3 v3Start = v3CameraPos;
    float fDepth = exp((fInnerRadius - fCameraHeight) * fInvScaleDepth);
    float fCameraAngle = dot(-v3Ray, v3Pos);
    float fLightAngle = dot(p_ldir, v3Pos);
    float fCameraScale = scale(fCameraAngle);
    float fLightScale = scale(fLightAngle);
    float fCameraOffset = fDepth * fCameraScale;
    float fTemp = (fLightScale + fCameraScale);

    // Initialize the scattering loop variables
	//gl_FrontColor = vec4(0.0, 0.0, 0.0, 0.0);
    float fSampleLength = fFar / fSamples;
    float fScaledLength = fSampleLength * fScale;
    float3 v3SampleRay = v3Ray * fSampleLength;
    float3 v3SamplePoint = v3Start + v3SampleRay * 0.5;


	// Now loop through the sample rays
    float3 v3FrontColor = float3(0.0, 0.0, 0.0);
    float3 v3Attenuate;
    for (int i = 0; i < nSamples; i++)
    {
        float fHeight = length(v3SamplePoint);
        float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
        float fScatter = fDepth * fTemp - fCameraOffset;
        v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
        v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
        v3SamplePoint += v3SampleRay;
    }

    res.c0 = v3FrontColor * (v3InvWavelength * fKrESun + fKmESun);
    res.c1 = v3Attenuate;
    res.v3Direction = 0.0;

    return res;
}