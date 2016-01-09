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
m_perlinnoisebuffer_texture( NULL ),
m_perlinnoisemap_texture( NULL ),
m_pnbufftexture_content( NULL ),
m_pnmaptexture_content( NULL ),
m_pnbufftexture_data( NULL ),
m_pnmaptexture_data( NULL )
{
    m_renderer = SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    for( size_t i = 0; i < nbFractalSource; i++ )
    {
        m_fbmParams[i].m_fbmInputHalfRange = 10.0;
        m_fbmParams[i].m_fbmLacunarity = 2.0;
        m_fbmParams[i].m_fbmRoughness = 0.5;
        m_fbmParams[i].m_fbmClamp = true;
        m_fbmParams[i].m_fbmClipMode = 1.0;
        m_fbmParams[i].m_fbmClipValue = 0.0;
        m_fbmParams[i].m_fbmSeed = 1;

        m_fractal[i] = NULL;
    }
}

LandscapeMultiFbm::~LandscapeMultiFbm( void )
{
    for( size_t i = 0; i < nbFractalSource; i++ )
    {
        if( m_fractal[i] )
        {
            _DRAWSPACE_DELETE_( m_fractal[i] );
        }
    }
}


void LandscapeMultiFbm::InitialiseResources( void )
{
    for( size_t i = 0; i < nbFractalSource; i++ )
    {
        m_fractal[i] = _DRAWSPACE_NEW_( Fractal, Fractal( 3, m_fbmParams[i].m_fbmSeed, 
                                    m_fbmParams[i].m_fbmRoughness, m_fbmParams[i].m_fbmLacunarity ) );    
    }

    m_perlinnoisebuffer_texture = new Texture();    
    m_perlinnoisebuffer_texture->SetFormat( 256, 3, 4 );
    m_perlinnoisebuffer_texture->SetPurpose( Texture::PURPOSE_FLOAT );

    m_perlinnoisemap_texture = new Texture();
    m_perlinnoisemap_texture->SetFormat( 256, 1, 4 );
    m_perlinnoisemap_texture->SetPurpose( Texture::PURPOSE_FLOAT );

    if( false == m_renderer->CreateTexture( m_perlinnoisebuffer_texture, &m_pnbufftexture_data ) )
    {
        _DSEXCEPTION( "failed to create perlin noise buffer texture in renderer" );
    }

    if( false == m_renderer->CreateTexture( m_perlinnoisemap_texture, &m_pnmaptexture_data ) )
    {
        _DSEXCEPTION( "failed to create perlin noise map texture in renderer" );
    }

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
            float temp = m_fractal[0]->GetNBuffer( i, j );
            *float_ptr = temp; float_ptr++;
        }
    }

    float_ptr = (float*)m_pnmaptexture_content;

    for( long i = 0; i < 256; i++ )
    {
        *float_ptr = m_fractal[0]->GetNMap( i ); float_ptr++;
    }

    m_perlinnoisemap_texture->UpdateTextureContent();
    m_perlinnoisebuffer_texture->UpdateTextureContent();
}

void LandscapeMultiFbm::BindShadersParams( void )
{
    Vector fbm_params;
    fbm_params[0] = m_fbmParams[0].m_fbmLacunarity;
    fbm_params[1] = m_fbmParams[0].m_fbmInputHalfRange;
    fbm_params[2] = ( m_fbmParams[0].m_fbmClamp ? 1.0 : 0.0 );

    Vector fbm_params2;
    fbm_params2[0] = m_fbmParams[0].m_fbmClipMode;
    fbm_params2[1] = m_fbmParams[0].m_fbmClipValue;
    fbm_params2[2] = m_fbmParams[0].m_fbmRoughness;

    m_renderer->SetFxShaderParams( 0, 27, fbm_params );
    m_renderer->SetFxShaderParams( 0, 28, fbm_params2 );
}

void LandscapeMultiFbm::BindTextures( void )
{
    m_renderer->SetVertexTexture( m_pnbufftexture_data, 0 );
    m_renderer->SetVertexTexture( m_pnmaptexture_data, 1 );
}

void LandscapeMultiFbm::UnbindTextures( void )
{
    m_renderer->UnsetVertexTexture( 0 );
    m_renderer->UnsetVertexTexture( 1 );
}