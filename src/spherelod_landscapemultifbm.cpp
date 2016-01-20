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

#include "spherelod_landscapemultifbm.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::SphericalLOD;

LandscapeMultiFbm::LandscapeMultiFbm( void ) :
m_mask_seed1( 500.0 ),
m_mask_seed2( 600.0 ),
m_mask_input_half_range( 20.0 ),
m_mountains_lacunarity( 2.0 ),
m_mountains_roughness( 0.25 ),
m_mountains_input_half_range( 8.0 ),
m_mountains_amplitude( 4000.0 ),
m_mountains_seed1( 500 ),
m_mountains_seed2( 600 ),
m_plains_lacunarity( 2.6 ),
m_plains_roughness( 0.5 ),
m_plains_input_half_range( 0.8 ),
m_plains_amplitude( 1000.0 ),
m_plains_seed1( 500.0 ),
m_plains_seed2( 600.0 )
{
    m_renderer = SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
}

LandscapeMultiFbm::~LandscapeMultiFbm( void )
{
}

void LandscapeMultiFbm::InitialiseResources( void )
{
}

void LandscapeMultiFbm::BindShadersParams( void )
{
    Vector fbm_params;
    Vector fbm_params2;

    Vector fbm_params3;
    Vector fbm_params4;

    Vector fbm_params5;
    Vector fbm_params6;

  
    fbm_params[1] = m_mask_input_half_range;
    fbm_params2[0] = m_mask_seed1;
    fbm_params2[1] = m_mask_seed2;



    fbm_params3[0] = m_mountains_lacunarity;
    fbm_params3[1] = m_mountains_input_half_range;
    fbm_params3[3] = m_mountains_amplitude;

    fbm_params4[0] = m_mountains_seed1;
    fbm_params4[1] = m_mountains_seed2;
    fbm_params4[2] = m_mountains_roughness;


    fbm_params5[0] = m_plains_lacunarity;
    fbm_params5[1] = m_plains_input_half_range;
    fbm_params5[3] = m_plains_amplitude;

    fbm_params6[0] = m_plains_seed1;
    fbm_params6[1] = m_plains_seed2;
    fbm_params6[2] = m_plains_roughness;


    m_renderer->SetFxShaderParams( 0, 27, fbm_params );
    m_renderer->SetFxShaderParams( 0, 28, fbm_params2 );

    m_renderer->SetFxShaderParams( 0, 29, fbm_params3 );
    m_renderer->SetFxShaderParams( 0, 30, fbm_params4 );

    m_renderer->SetFxShaderParams( 0, 31, fbm_params5 );
    m_renderer->SetFxShaderParams( 0, 32, fbm_params6 );
}

void LandscapeMultiFbm::BindTextures( void )
{
}


void LandscapeMultiFbm::UnbindTextures( void )
{
}