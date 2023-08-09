/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#include "texture.h"
#include "file.h"
#include "memalloc.h"
#include "exceptions.h"
#include "misc_utils.h"
#include "renderer.h"
#include "plugin.h"
#include "md5.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

dsstring Texture::m_rootpath = ".";


Texture::Texture( void ) :
m_filedata( NULL ), 
m_filedatasize( -1 ), 
m_render_target( false ),
m_render_target_width( 256 ),
m_render_target_height( 256 ),
m_renderpurpose( RENDERPURPOSE_COLOR ),
m_rendertarget( RENDERTARGET_GPU ),
m_purpose( PURPOSE_COLORFROMFILE ),
m_render_data( NULL ),
m_width( 256 ),
m_height( 256 ),
m_depth( 0 ),
m_bpp( 4 ),
m_dataowner(false)
{
}

Texture::Texture( const dsstring& p_path, bool p_render_target, unsigned long p_render_target_width, unsigned long p_render_target_height, 
            RenderPurpose p_rp, RenderTarget p_rt ) : 
m_path( p_path ), 
m_filedata( NULL ), 
m_filedatasize( -1 ), 
m_render_target( p_render_target ),
m_render_target_width( p_render_target_width ),
m_render_target_height( p_render_target_height ),
m_renderpurpose( p_rp ),
m_rendertarget( p_rt ),
m_purpose( PURPOSE_COLORFROMFILE ),
m_render_data( NULL ),
m_width( 256 ),
m_height( 256 ),
m_depth(0),
m_bpp( 4 ),
m_dataowner(false)
{
}

Texture::~Texture( void )
{
    ReleaseData();
}

bool Texture::LoadFromFile(void)
{
    if (m_filedata)
    {
        // nettoyer
        ReleaseData();
    }

    if ("" == m_path)
    {
        _DSEXCEPTION("Texture filepath not initialized!");
    }

    long size;
    void* data = File::loadAndAllocBinaryFile(compute_final_path(), &size);
    if (!data)
    {
        return false;
    }
    m_filedata = data;
    m_filedatasize = size;

    m_dataowner = true;
    return true;
}

void Texture::ReleaseData(void)
{
    if (m_filedata && m_dataowner)
    {
        _DRAWSPACE_DELETE_N_(m_filedata);
        m_filedata = NULL;
        m_filedatasize = -1;
    }
}

void* Texture::GetData( void ) const
{
    return m_filedata;
}

long Texture::GetDataSize( void ) const
{
    return m_filedatasize;
}

void Texture::GetPath( dsstring& p_path ) const
{
    p_path = compute_final_path();
}

void Texture::GetBasePath(dsstring& p_path) const
{
    p_path = m_path;
}

bool Texture::IsRenderTarget( void ) const
{
    return m_render_target;
}

bool Texture::Is3DTexture(void) const
{
    return (m_depth > 0);
}

void Texture::GetRenderTargetDims( unsigned long& p_w, unsigned long& p_h ) const
{
    p_w = m_render_target_width;
    p_h = m_render_target_height;
}

void Texture::SetFormat( long p_width, long p_height, long p_bpp )
{
    m_width = p_width;
    m_height = p_height;

    m_bpp = p_bpp;
}

void Texture::SetDepth(long p_depth)
{
    m_depth = p_depth;
}

void Texture::SetRenderData( void* p_render_data )
{
    if( m_render_data != NULL )
    {
        if( p_render_data && p_render_data != m_render_data )
        {
            // pas normal...
            _DSEXCEPTION( "texture previously created in renderer !!" );
        }
    }
    m_render_data = p_render_data;
}

void Texture::GetFormat( long& p_width, long& p_height, long& p_bpp ) const
{
    p_width = m_width;
    p_height = m_height;
    p_bpp = m_bpp;
}

long Texture::GetDepth(void) const
{
    return m_depth;
}

Texture::RenderPurpose Texture::GetRenderPurpose( void ) const
{
    return m_renderpurpose;
}

Texture::RenderTarget Texture::GetRenderTarget( void ) const
{
    return m_rendertarget;
}

Texture::Purpose Texture::GetPurpose( void ) const
{
    return m_purpose;
}

void Texture::SetPurpose( Texture::Purpose p_purpose )
{
    m_purpose = p_purpose;
}

bool Texture::AllocTextureContent( void )
{
    if( !m_render_data )
    {
        return false;
    }

    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;
    return renderer->AllocTextureContent( m_render_data );    
}

void Texture::ReleaseTextureContent( void )
{
    if( !m_render_data )
    {
        return;
    }

    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;
    renderer->ReleaseTextureContent( m_render_data );
}

void* Texture::GetTextureContentPtr( void ) const
{
    if( !m_render_data )
    {
        return false;
    }

    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;
    return renderer->GetTextureContentPtr( m_render_data );
}

bool Texture::CopyTextureContent( void )
{
    if( !m_render_data )
    {
        return false;
    }

    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;
    return renderer->CopyTextureContent( m_render_data );
}

bool Texture::UpdateTextureContent( void )
{
    if( !m_render_data )
    {
        return false;
    }

    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;
    return renderer->UpdateTextureContent( m_render_data );
}

void* Texture::GetRenderData( void ) const
{
    return m_render_data;
}


dsstring Texture::compute_final_path(void) const
{
    dsstring final_path = m_rootpath + "/";

    final_path += m_path;
    return final_path;
}

void Texture::SetRootPath(const dsstring& p_path)
{
    m_rootpath = p_path;
}

void Texture::GetMD5( dsstring& p_md5 ) const
{
    MD5 md5;

    dsstring hash_path = md5.digestMemory( (BYTE*)m_path.c_str(), (int)( m_path.size() ) );

    unsigned long rt_infos[3];
    rt_infos[0] = (unsigned long)m_render_target;
    rt_infos[1] = m_render_target_width;
    rt_infos[2] = m_render_target_height;

    dsstring hash_rt_infos = md5.digestMemory( (BYTE*)rt_infos, (int)( 3 * sizeof( unsigned long ) ) );

    unsigned char modes_infos[3];
    modes_infos[0] = (unsigned char)m_renderpurpose;
    modes_infos[1] = (unsigned char)m_rendertarget;
    modes_infos[2] = (unsigned char)m_purpose;

    dsstring hash_modes_infos = md5.digestMemory( (BYTE*)modes_infos, (int)( 3 * sizeof( unsigned char ) ) );


    p_md5 = hash_path + hash_rt_infos + hash_modes_infos;

    if( !IsRenderTarget() && GetPurpose() != PURPOSE_COLORFROMFILE )
    {
        // prendre aussi en compte m_width, m_height et m_bpp, qui servent de parametres d'entrees dans ce cas

        unsigned long format_infos[3];
        format_infos[0] = (unsigned long)m_width;
        format_infos[1] = (unsigned long)m_height;
        format_infos[2] = (unsigned long)m_bpp;

        dsstring hash_ft_infos = md5.digestMemory( (BYTE*)format_infos, (int)( 3 * sizeof( unsigned long ) ) );

        p_md5 += hash_ft_infos;
    }
}

void Texture::SetData(void* p_data, long p_size)
{
    m_filedata = p_data;
    m_filedatasize = p_size;
}
