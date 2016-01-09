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
}

void LandscapeMultiFbm::BindShadersParams( void )
{
    Vector fbm_params;

    fbm_params[0] = m_fbm.m_Lacunarity;
    fbm_params[1] = m_fbm.m_InputHalfRange;
    fbm_params[2] = ( m_fbm.m_Clamp ? 1.0 : 0.0 );


    Vector fbm_params2;

    fbm_params2[0] = m_fbm.m_ClipMode;
    fbm_params2[1] = m_fbm.m_ClipValue;
    fbm_params2[2] = m_fbm.m_Roughness;


    m_renderer->SetFxShaderParams( 0, 27, fbm_params );
    m_renderer->SetFxShaderParams( 0, 28, fbm_params2 );
}

void LandscapeMultiFbm::BindTextures( void )
{
    m_renderer->SetVertexTexture( m_fbm.m_pnbufftexture_data, 0 );
    m_renderer->SetVertexTexture( m_fbm.m_pnmaptexture_data, 1 );
    
}

void LandscapeMultiFbm::UnbindTextures( void )
{
    m_renderer->UnsetVertexTexture( 0 );
    m_renderer->UnsetVertexTexture( 1 );
}