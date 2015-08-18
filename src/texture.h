/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

    } RenderPurpose;

protected:
    dsstring        m_path;
    long            m_width;
    long            m_height;
    long            m_bpp;
    void*           m_render_data;

    void*           m_filedata;
    long            m_filedatasize;

    bool            m_render_target;
    unsigned long   m_render_target_width;
    unsigned long   m_render_target_height;

    RenderPurpose   m_renderpurpose;

    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );
    
public:
    Texture( void );
    Texture( const dsstring& p_path, bool p_render_target = false, unsigned long p_render_target_width = 256, unsigned long p_render_target_height = 256, RenderPurpose p_rp = RENDERPURPOSE_COLOR );
    ~Texture( void );

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

    void GetKeyword( dsstring& p_outkeyword );

    bool AllocTextureContent( void );
    void ReleaseTextureContent( void );
    void* GetTextureContentPtr( void );
    bool CopyTextureContent( void );

    static Asset* Instanciate( void );

};
}
}

#endif
