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

LandscapeMultiFbm::LandscapeMultiFbm( void )
{
    m_renderer = SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
}

LandscapeMultiFbm::~LandscapeMultiFbm( void )
{
}

void LandscapeMultiFbm::InitialiseResources( void )
{
    m_fbm.Initialise();
    m_fbm2.Initialise();
    m_fbm3.Initialise();
}

void LandscapeMultiFbm::BindShadersParams( void )
{
    Vector fbm_params;
    Vector fbm_params2;

    Vector fbm_params3;
    Vector fbm_params4;

    Vector fbm_params5;
    Vector fbm_params6;


    fbm_params[0] = m_fbm.m_Lacunarity;
    fbm_params[1] = m_fbm.m_InputHalfRange;
    fbm_params[2] = ( m_fbm.m_Clamp ? 1.0 : 0.0 );
    fbm_params[3] = m_fbm.m_Amplitude;

    fbm_params2[0] = m_fbm.m_Seed1;
    fbm_params2[1] = m_fbm.m_Seed2;
    fbm_params2[2] = m_fbm.m_Roughness;


    fbm_params3[0] = m_fbm2.m_Lacunarity;
    fbm_params3[1] = m_fbm2.m_InputHalfRange;
    fbm_params3[2] = ( m_fbm2.m_Clamp ? 1.0 : 0.0 );
    fbm_params3[3] = m_fbm2.m_Amplitude;

    fbm_params4[0] = m_fbm2.m_Seed1;
    fbm_params4[1] = m_fbm2.m_Seed2;
    fbm_params4[2] = m_fbm2.m_Roughness;


    fbm_params5[0] = m_fbm3.m_Lacunarity;
    fbm_params5[1] = m_fbm3.m_InputHalfRange;
    fbm_params5[2] = ( m_fbm3.m_Clamp ? 1.0 : 0.0 );
    fbm_params5[3] = m_fbm3.m_Amplitude;

    fbm_params6[0] = m_fbm3.m_Seed1;
    fbm_params6[1] = m_fbm3.m_Seed2;
    fbm_params6[2] = m_fbm3.m_Roughness;


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