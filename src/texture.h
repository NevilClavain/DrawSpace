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

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "drawspace_commons.h"
#include "asset.h"

#define TEXTURE_TEXT_KEYWORD    "Texture"
#define TEXTURE_ARC_MAGICNUMBER 0x1066

namespace DrawSpace
{
namespace Core
{

class Texture : public Asset
{
public:

    typedef enum
    {
        RENDERPURPOSE_COLOR,
        RENDERPURPOSE_FLOAT,
        RENDERPURPOSE_FLOAT32,
        RENDERPURPOSE_FLOATVECTOR,

    } RenderPurpose;

    typedef enum
    {
        RENDERTARGET_GPU,  // cas ou la texture resultante va �tre bindee a un shader (GPU) (d�faut)
        RENDERTARGET_CPU,  // cas ou le contenu de la texture resultante doit �tre lue par le CPU

    } RenderTarget;


    typedef enum
    {
        PURPOSE_COLORFROMFILE,
        PURPOSE_COLOR,
        PURPOSE_FLOAT32,
        PURPOSE_FLOAT32VECTOR,

    } Purpose;

protected:

    static dsstring                 m_rootpath;

    dsstring                        m_path;
    long                            m_width;
    long                            m_height;
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

    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

    dsstring compute_final_path( void );
    
public:
    Texture( void );
    Texture( const dsstring& p_path, bool p_render_target = false, unsigned long p_render_target_width = 256, unsigned long p_render_target_height = 256, 
                RenderPurpose p_rp = RENDERPURPOSE_COLOR, RenderTarget p_rt = RENDERTARGET_GPU );
    ~Texture( void );

    static void SetRootPath( const dsstring& p_path );

    bool LoadFromFile( void );
    void ReleaseData( void );

    void* GetData( void );
    long GetDataSize( void );

    void GetPath( dsstring& p_path );

    virtual bool ApplyProperties( void );

    void Serialize( Utils::Archive& p_archive );
    bool Unserialize( Utils::Archive& p_archive );

    void DumpProperties( dsstring& p_text );
    bool ParseProperties( const dsstring& p_text );

    bool IsRenderTarget( void );
    void GetRenderTargetDims( unsigned long& p_w, unsigned long& p_h );

    void SetFormat( long p_width, long p_height, long p_bpp );
    void SetRenderData( void* p_renderdata );
    void GetFormat( long& p_width, long& p_height, long& p_bpp );

    RenderPurpose GetRenderPurpose( void );
    RenderTarget GetRenderTarget( void );
    Purpose GetPurpose( void );
    void SetPurpose( Purpose p_purpose );

    void GetKeyword( dsstring& p_outkeyword );

    bool AllocTextureContent( void );
    void ReleaseTextureContent( void );
    void* GetTextureContentPtr( void );
    bool CopyTextureContent( void );
    bool UpdateTextureContent( void );

    void* GetRenderData( void );

    static Asset* Instanciate( void );

};
}
}

#endif
