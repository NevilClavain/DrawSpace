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

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "drawspace_commons.h"
#include "asset.h"

namespace DrawSpace
{
namespace Core
{

class Texture : public Asset
{
protected:
    dsstring        m_path;
    long            m_width;
    long            m_height;
    long            m_bpp;

    void*           m_filedata;
    long            m_filedatasize;

    bool            m_render_target;
    unsigned long   m_render_target_width;
    unsigned long   m_render_target_height;
    
public:
    Texture( const dsstring& p_path, bool p_render_target = false, unsigned long p_render_target_width = 256, unsigned long p_render_target_height = 256 );
    ~Texture( void );

    bool LoadFromFile( void );
    void ReleaseData( void );

    void* GetData( void );
    long GetDataSize( void );

    void GetPath( dsstring& p_path );

    virtual bool Load( void );

    void Serialize( Utils::Archive& p_archive );
    void Unserialize( Utils::Archive& p_archive );

    bool IsRenderTarget( void );
    void GetRenderTargetDims( unsigned long& p_w, unsigned long& p_h );

    void SetFormat( long p_width, long p_height, long p_bpp );
    void GetFormat( long& p_width, long& p_height, long& p_bpp );

};
}
}

#endif