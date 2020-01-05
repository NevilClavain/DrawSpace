/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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
float4x4 matWorldView:           register(c4);

float    fogDensity:             register(c24);


struct VS_INPUT 
{
   float4 Position : POSITION0;      
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float  Fog      : FOG;
};

float ComputeExp2Fog( float4 worldViewPos, float density )
{
   float4 org;
   
   org.x = 0.0;
   org.y = 0.0;
   org.z = 0.0;
   org.w = 1.0;
   
   //float4 d = distance( org, worldViewPos );
   float4 d = - worldViewPos[2];
   return 1 / exp2( d * density );
   
   // retour a 0.0 -> brouillard au maximum
}


VS_OUTPUT vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output;
   Output.Position = mul( Input.Position, matWorldViewProjection );  
      
   float4 Position = mul( Input.Position, matWorldView );   
   Output.Fog = clamp( 0.0, 1.0, ComputeExp2Fog( Position, fogDensity ) );       
   return( Output );   
}
