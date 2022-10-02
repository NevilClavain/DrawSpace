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

#include "mat_input_constants.hlsl"

#define v_flag0                     24
#define v_patch_translation         25
#define v_base_uv                   26
#define v_base_uv_global            27


#define v_landscape_control         40
#define v_seeds                     41


///////////////////////////////////// Inputs description ///////////////////////


#define v_thparams                  42
// x -> humidity_alt_max : en metres : plus ce seuil altitude sera haut, plus l'humidite pourra se repandre
// y -> not used
// z -> temperature_alt_dec : en degres centigrades; taux de decrementation de temperature au fur et a mesure de l'augmentation de l'altitude -> nombre de degres perdus tout les 1000 mètres
// w -> beach_lim, en mètres

#define v_thparams2                 43
// x -> lim polar : en latitude "normalisée" : 0.0 = pole, 1.0 = equateur
// y -> lim tropical : en latitude "normalisée" : 0.0 = pole, 1.0 = equateur
// z -> k polar : de 0.0 a 1.0
// w -> k tropical : de 0.0 a 1.0

struct VS_INPUT
{
    float3 Position : POSITION;
    float4 TexCoord0 : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 aht : TEXCOORD0; // aht : altitude temperature humidity
};


#include "fbm.hlsl"
#include "multifbm_height.hlsl"
#include "spherelod_commons.hlsl"

VS_OUTPUT vs_main(VS_INPUT Input)
{
    
    VS_OUTPUT Output;

    
    float4 flag0 = vec[v_flag0];
    float4 patch_translation = vec[v_patch_translation];
    float4 base_uv = vec[v_base_uv];
    float4 base_uv_global = vec[v_base_uv_global];
    float4 landscape_control = vec[v_landscape_control];
    float4 seeds = vec[v_seeds];
    float4 thparams = vec[v_thparams];
    float4 thparams2 = vec[v_thparams2];

    float4 v_position;

    float temperature_min = -50.0;

    float temperature_max = 27.0;

	//float temperature_alt_dec = 0.0064;  // INPUT 
    float temperature_alt_dec = thparams.z / 1000.0;
	//pente temperature fct de l'altitude : x ? perdus tout les 1000 metres 

    float temperature_lat;
    float temperature_alt;
    float temperature_final;

		
	// sidelenght scaling

    v_position.xyz = Input.Position * flag0.y / 2.0;
    v_position.xy = v_position.xy + patch_translation.xy;
    v_position.z = 1.0;
    v_position.w = 1.0;
	
    float4 v_position2;
    v_position2.w = 1.0;
    v_position2.xyz = CubeToSphere(ProjectVectorToCube(flag0.x, v_position.xyz));
	
    float h = sqrt((v_position2.x * v_position2.x) + (v_position2.z * v_position2.z)); // h compris entre 0.0 et 1.0


    float vertex_latitude = acos(clamp( h, 0.0, 1.0 ));

    float vert_alt = ComputeVertexHeight(v_position2, landscape_control.x, landscape_control.y, landscape_control.z, landscape_control.w, seeds.x, seeds.y, seeds.z, seeds.w);    
    // vert_alt : altitude en metres

    float4 global_uv = 0.0;
    global_uv.x = lerp(base_uv_global.x, base_uv_global.z, Input.TexCoord0.x);
    global_uv.y = lerp(base_uv_global.y, base_uv_global.w, Input.TexCoord0.y);
    
    /*
    vert_alt += ComputeCanyonsFromTexture(TextureCanyons, v_position2, global_uv, seeds.z, seeds.w);
    vert_alt *= ComputeRiversFromTexture(TextureRivers, v_position2, global_uv, seeds.z, seeds.w);
    */
    
    Output.aht = 0.0;
    Output.aht.x = vert_alt;




	///////////////////// calcul facteur temperature


	// calcul temperature en fct de la lattitude du vertex


    float norm_latitude = clamp( 1.0 - (2.0 * vertex_latitude / 3.1415927), 0.0, 1.0 );

    // pole : vertex_latitude = PI/2 -> norm_latitude = 0.0
    // equateur : vertex_latitude = 0 -> norm_latitude = 1.0



    float temp_x;

	////////////////////////////////////////// INPUT

    float lim_polar = thparams2.x;
    float lim_tropical = thparams2.y;
    float k_polar = thparams2.z;
    float k_tropical = thparams2.w;

	//////////////////////////////////////////

    if (norm_latitude < lim_polar)
    {
		// zone polaire

        temp_x = k_polar * norm_latitude / lim_polar;
    }
    else if (norm_latitude < lim_tropical)
    {
		// zone temperee

        temp_x = k_polar + (k_tropical - k_polar) * (norm_latitude - lim_polar) / (lim_tropical - lim_polar);
    }
    else
    {
		// zone tropicale

        temp_x = k_tropical + (1.0 - k_tropical) * (norm_latitude - lim_tropical) / (1.0 - lim_tropical);
    }



    temperature_lat = ((temperature_max - temperature_min) * temp_x) + temperature_min;

    // temp_x = 0.0 -> temperature_lat = temperature_min
    // temp_x = 1.0 -> temperature_lat = temperature_max
	
	// calcul de la baisse de temperature en fct de l'altitude
    //float temp_dec = temperature_alt_dec * clamp(2.0 * pn_temperature_variation, 0.6, 2.0) * vert_alt;
    float temp_dec = temperature_alt_dec * vert_alt;

    // vert_alt = 0 -> temp_dec = 0 -> temperature_alt = temperature_lat
    // sinon temperature_alt <<< temperature_lat
    temperature_alt = temperature_lat - temp_dec;

    temperature_final = clamp(temperature_alt, temperature_min, temperature_max);
    float color_temp = (temperature_final - temperature_min) / (temperature_max - temperature_min);

    // -> color_temp : compris entre 0.0 et 1.0



    ///////////////////// calcul facteur humidite

    float color_humidity;
    float color_humidity_final;

    float humidity_alt_max = thparams.x;

    float vert_alt_2 = clamp(vert_alt, 0.0, humidity_alt_max);
	
    float hf = 1.0 - (vert_alt_2 / humidity_alt_max);

    // vertice élevé -> hf = 0
    // vertice altitude 0 -> hf = 1

    color_humidity = lerp(1.0 - hf, hf, color_temp);

    // color_temp (temperature) élevée
    //      -> vertex = 0.0 : humidité élevée
    //      -> vertex = humidity_alt_max : humidité faible
    // 
    // color_temp (temperature) basse
    //      -> vertex = 0.0 : humidité faible
    //      -> vertex = humidity_alt_max : humidité élevée

    // seuil humidity_alt_max élevé -> l'humidité se "repandra" plus facilement

    
    float beach_lim = norm_latitude * thparams.w;

    if (vert_alt > beach_lim)
    {
        color_humidity_final = color_humidity;
    }
    else if (vert_alt <= beach_lim && vert_alt > 0.0)
    {
        color_humidity_final = lerp(0.0, color_humidity, vert_alt * 0.5);
    }
    else
    {
        color_humidity_final = 0.0;
    }


    Output.aht.y = color_temp;
    Output.aht.z = color_humidity_final;

    

    float4 pos;
    pos.xyz = Input.Position;
    pos.w = 1.0;
    Output.Position = mul(pos, mat[matWorldViewProjection]);
    				  
    return (Output);
}
