/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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


float4x4 matWorldViewProjection: register(c0);
float4   flag0:				register(c24);

	// .x -> patch orientation enum integer
	// .y -> patch sidelenght
	// .z -> planet ray

float4   patch_translation:	register(c25);
	/// .x, .y -> patch positionning


sampler2D Texture0 : register(s0);

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float4 TexCoord0: TEXCOORD0;   
      
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float4 TexCoord0: TEXCOORD0;   
};


VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;

	float4 v_position;
	
	float4 textcoord = Input.TexCoord0;
	textcoord.w = 1.0;
	
	float4 v_alt = tex2Dlod( Texture0, textcoord );
	
	// sidelenght scaling

	v_position = Input.Position * flag0.y / 2.0;
	v_position = v_position + patch_translation;
		
	v_position.w = 1.0;
	
	float4 v_position2;

	if( 0.0 == flag0.x ) // front
	{
		v_position2.x = v_position.x;
		v_position2.y = v_position.y;
		v_position2.z = 1.0;   
	}
	else if( 1.0 == flag0.x ) // rear
	{
		v_position2.x = -v_position.x;
		v_position2.y = v_position.y;
		v_position2.z = -1.0;   
	}
	else if( 2.0 == flag0.x ) // left
	{
		v_position2.x = -1.0;
		v_position2.y = v_position.y;
		v_position2.z = v_position.x;   
	}
	else if( 3.0 == flag0.x ) // right
	{
		v_position2.x = 1.0;
		v_position2.y = v_position.y;
		v_position2.z = -v_position.x;   
	}
	else if( 4.0 == flag0.x ) // top
	{
		v_position2.x = v_position.x;
		v_position2.y = 1.0;
		v_position2.z = -v_position.y;   
	}
	else //if( 5.0 == flag0.x ) // bottom
	{
		v_position2.x = v_position.x;
		v_position2.y = -1.0;
		v_position2.z = v_position.y;
	}

	v_position2.w = v_position.w;

	float xtemp = v_position2.x;
	float ytemp = v_position2.y;
	float ztemp = v_position2.z;

	v_position2.x = xtemp * sqrt( 1.0 - ytemp * ytemp * 0.5 - ztemp * ztemp * 0.5 + ytemp * ytemp * ztemp * ztemp / 3.0 );
	v_position2.y = ytemp * sqrt( 1.0 - ztemp * ztemp * 0.5 - xtemp * xtemp * 0.5 + xtemp * xtemp * ztemp * ztemp / 3.0 );
	v_position2.z = ztemp * sqrt( 1.0 - xtemp * xtemp * 0.5 - ytemp * ytemp * 0.5 + xtemp * xtemp * ytemp * ytemp / 3.0 );
	
	v_position2 = v_position2 * ( 1.0 + ( v_alt.x * 0.001 ) );
	   
   
	// final scaling
	float4 v_position3;	
	v_position3 = v_position2 * flag0.z;	
	v_position3.w = 1.0;
	

	Output.Position = mul( v_position3, matWorldViewProjection );
	Output.TexCoord0 = Input.TexCoord0;
			  
	return( Output );   
}
