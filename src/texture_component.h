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

#ifndef _TEXTURE_COMPONENT_H_
#define _TEXTURE_COMPONENT_H_

#include "component.h"
#include "components_ids.h"
#include "texture.h"

namespace DrawSpace
{
struct TextureComponent : public ComponentBase
{
    Core::Texture*                  m_texture;
    dsstring                        m_path;
    bool                            m_enable_render_target;
    unsigned long                   m_render_target_width;
    unsigned long                   m_render_target_height;
    Core::Texture::RenderPurpose    m_render_purpose;
    Core::Texture::Purpose          m_purpose;
    Core::Texture::RenderTarget     m_render_target;


    TextureComponent( void ) :
    m_texture( NULL ),
    m_enable_render_target( false ),
    m_render_target_width( 256 ),
    m_render_target_height( 256 ),
    m_render_purpose( Core::Texture::RENDERPURPOSE_COLOR ),
    m_purpose( Core::Texture::PURPOSE_COLORFROMFILE ),
    m_render_target( Core::Texture::RENDERTARGET_GPU )
    {
        m_type = TextureComponentType;
    }
};
}

#endif

