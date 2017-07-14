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

#ifndef _RENDERTARGET_COMPONENT_H_
#define _RENDERTARGET_COMPONENT_H_

#include "component.h"
#include "texture.h"
#include "components_ids.h"

namespace DrawSpace
{
struct RenderTargetComponent : public ComponentBase
{

    Core::Texture*                  m_targettexture;
    dsstring                        m_name;

    bool                            m_targetdims_fromrenderer;
    long                            m_targetdims_width;
    long                            m_targetdims_height;

    Core::Texture::RenderPurpose    m_renderpurpose;
    Core::Texture::RenderTarget     m_rendertarget;

    RenderTargetComponent( const dsstring& p_name ) :
    m_name( p_name ),
    m_targettexture( NULL ),
    m_targetdims_fromrenderer( true ),
    m_targetdims_width( 256 ),
    m_targetdims_height( 256 ),
    m_renderpurpose( Core::Texture::RENDERPURPOSE_COLOR ),
    m_rendertarget( Core::Texture::RENDERTARGET_GPU )
    {
        m_type = RenderTargetComponentType;
    }
};
}

#endif