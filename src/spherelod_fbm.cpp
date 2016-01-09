
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

#include "spherelod_fbm.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::SphericalLOD;

Fbm::Fbm( void ) :
m_fbmInputHalfRange( 10.0 ),
m_fbmLacunarity( 2.0 ),
m_fbmRoughness( 0.5 ),
m_fbmClamp( true) ,
m_fbmClipMode( 1.0 ),
m_fbmClipValue( 0.0 ),
m_fbmSeed( 1 ),
m_fractal( NULL )
{
    m_renderer = SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
}

Fbm::~Fbm( void )
{
    if( m_fractal )
    {
        _DRAWSPACE_DELETE_( m_fractal );
    }   
}

void Fbm::Initialise( void )
{
    m_fractal = _DRAWSPACE_NEW_( Fractal, Fractal( 3, m_fbmSeed, m_fbmRoughness, m_fbmLacunarity ) );

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
