/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "texture.h"
#include "file.h"
#include "memalloc.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Texture::Texture( const dsstring& p_path, bool p_render_target, unsigned long p_render_targer_width, unsigned long p_render_targer_height ) : 
m_path( p_path ), 
m_data( NULL ), 
m_datasize( -1 ), 
m_render_target( p_render_target ),
m_render_targer_width( p_render_targer_width ),
m_render_targer_height( p_render_targer_height )
{
}

Texture::~Texture( void )
{
}

bool Texture::LoadFromFile( void )
{
    long size;
    void* data = File::LoadAndAllocBinaryFile( m_path, &size );
    if( !data )
    {
        return false;
    }
    m_data = data;
    m_datasize = size;
    return true;
}

void Texture::ReleaseData( void )
{
    if( m_data )
    {
        _DRAWSPACE_DELETE_N_( m_data );
        m_data = NULL;
    }
}

void* Texture::GetData( void )
{
    return m_data;
}

long Texture::GetDataSize( void )
{
    return m_datasize;
}

void Texture::GetPath( dsstring& p_path )
{
    p_path = m_path;
}

bool Texture::IsRenderTarget( void )
{
    return m_render_target;
}

void Texture::GetRenderTargetDims( unsigned long& p_w, unsigned long& p_h )
{
    p_w = m_render_targer_width;
    p_h = m_render_targer_height;
}

void Texture::Serialize( Factory& p_factory, Archive& p_archive )
{

}

void Texture::Unserialize( Factory& p_factory, Archive& p_archive )
{

}

