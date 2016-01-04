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

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::SphericalLOD;

LandscapeMultiFbm::LandscapeMultiFbm( DrawSpace::Utils::Fractal* p_fractal ) :
m_fractal( p_fractal ),
m_perlinnoisebuffer_texture( NULL ),
m_perlinnoisemap_texture( NULL ),
m_pnbufftexture_content( NULL ),
m_pnmaptexture_content( NULL )
{

}

LandscapeMultiFbm::~LandscapeMultiFbm( void )
{
}


void LandscapeMultiFbm::InitialiseResources( void )
{

    m_perlinnoisebuffer_texture = new Texture();    
    m_perlinnoisebuffer_texture->SetFormat( 256, 3, 4 );
    m_perlinnoisebuffer_texture->SetPurpose( Texture::PURPOSE_FLOAT );

    m_perlinnoisemap_texture = new Texture();
    m_perlinnoisemap_texture->SetFormat( 256, 1, 4 );
    m_perlinnoisemap_texture->SetPurpose( Texture::PURPOSE_FLOAT );

    // ICI creer les textures dans le renderer...

    m_perlinnoisebuffer_texture->AllocTextureContent();
    m_pnbufftexture_content = m_perlinnoisebuffer_texture->GetTextureContentPtr();

    m_perlinnoisemap_texture->AllocTextureContent();
    m_pnmaptexture_content = m_perlinnoisemap_texture->GetTextureContentPtr();
    
    unsigned char* color_ptr = (unsigned char*)m_pnmaptexture_content;
    float* float_ptr = (float*)m_pnbufftexture_content;
        
    for(long j = 0; j < 3; j++ )
    {
        for( long i = 0; i < 256; i++ )    
        {
            float temp = m_fractal->GetNBuffer( i, j );
            *float_ptr = temp; float_ptr++;
        }
    }

    float_ptr = (float*)m_pnmaptexture_content;

    for( long i = 0; i < 256; i++ )
    {
        *float_ptr = m_fractal->GetNMap( i ); float_ptr++;
    }

    m_perlinnoisemap_texture->UpdateTextureContent();
    m_perlinnoisebuffer_texture->UpdateTextureContent();
}

void LandscapeMultiFbm::BindShadersParams( void )
{

}

void LandscapeMultiFbm::BindTextures( void )
{

}
