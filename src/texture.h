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

#pragma once

#include "ds_types.h"

namespace DrawSpace
{
namespace Core
{

class Texture
{
public:

    //////////////////////////////////////////////////////////
    // pour les textures render targets

    using RenderPurpose = enum
    {
        RENDERPURPOSE_COLOR,
        RENDERPURPOSE_FLOAT,
        RENDERPURPOSE_FLOAT32,
        RENDERPURPOSE_FLOATVECTOR,
        RENDERPURPOSE_FLOATVECTOR32,

    };

    using RenderTarget = enum
    {
        RENDERTARGET_GPU,  // cas ou la texture resultante va etre bindee a un shader (GPU) (defaut)
        RENDERTARGET_CPU,  // cas ou le contenu de la texture resultante doit etre lue par le CPU

    };

    //////////////////////////////////////////////////////////
    // pour les textures qui ne sont pas des render targets
    using Purpose = enum
    {
        PURPOSE_COLORFROMFILE,
        PURPOSE_COLOR,
        PURPOSE_FLOAT32,
        PURPOSE_FLOAT32VECTOR,

    };

protected:

    static dsstring                 m_rootpath;

    dsstring                        m_path;
    long                            m_width;
    long                            m_height;
    long                            m_depth;

    long                            m_bpp;
    void*                           m_render_data;

    void*                           m_filedata;
    long                            m_filedatasize;

    bool                            m_render_target;
    unsigned long                   m_render_target_width;
    unsigned long                   m_render_target_height;

    RenderPurpose                   m_renderpurpose;
    RenderTarget                    m_rendertarget;
    Purpose                         m_purpose;

    bool                            m_dataowner;
    
    dsstring                        compute_final_path(void) const;
    
public:
    Texture( void );
    Texture( const dsstring& p_path, bool p_render_target = false, unsigned long p_render_target_width = 256, unsigned long p_render_target_height = 256, 
                RenderPurpose p_rp = RENDERPURPOSE_COLOR, RenderTarget p_rt = RENDERTARGET_GPU );
    ~Texture( void );

    
    void*           GetData(void) const;
    long            GetDataSize(void) const;
    void            GetPath(dsstring& p_path) const;
    void            GetBasePath(dsstring& p_path) const;
    void            GetRenderTargetDims(unsigned long& p_w, unsigned long& p_h) const;
    void            GetFormat(long& p_width, long& p_height, long& p_bpp) const;
    long            GetDepth(void) const;
    RenderPurpose   GetRenderPurpose(void) const;
    RenderTarget    GetRenderTarget(void) const;
    Purpose         GetPurpose(void) const;
    void*           GetTextureContentPtr(void) const;
    void*           GetRenderData(void) const;
    void            GetMD5(dsstring& p_md5) const;
    bool            IsRenderTarget(void) const;

    bool            Is3DTexture(void) const;

       
    void            SetFormat( long p_width, long p_height, long p_bpp );
    void            SetDepth(long p_depth);
    void            SetRenderData( void* p_renderdata );   
    void            SetPurpose( Purpose p_purpose );

    bool            AllocTextureContent( void );
    void            ReleaseTextureContent( void );
    
    bool            CopyTextureContent( void );
    bool            UpdateTextureContent( void );

    void            SetData( void* p_data, long p_size );

    static void     SetRootPath(const dsstring& p_path);

    bool            LoadFromFile(void);
    void            ReleaseData(void);

};
}
}

