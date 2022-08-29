/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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


#include "drawspace_commons.h"
#include "fx.h"
#include "texture.h"
#include "renderingnode.h"
#include "plugin.h"
#include "renderer.h"

namespace LOD
{

// cette classe peut etre vue comme un "descripteur" de renderingNode de planete (SphericalLOD::FaceDrawingNode)
struct Binder abstract
{
private:
    
    DrawSpace::Core::Texture*                       m_textures[DrawSpace::Core::RenderingNode::NbMaxTextures]; // 32 textures stages max
    DrawSpace::Core::Texture*                       m_vertextextures[DrawSpace::Core::RenderingNode::NbMaxTextures];

    DrawSpace::Core::Fx*                            m_fx{ nullptr };

protected:
    DrawSpace::Interface::Renderer*                 m_renderer{ nullptr };

public:

    Binder( void );

    virtual void Bind( void ) {}; // appelee juste avant le rendu du node
    virtual void Unbind( void ) {}; // appelee juste apres le rendu du node

    void SetRenderer( DrawSpace::Interface::Renderer* p_renderer );

    void SetTexture( DrawSpace::Core::Texture* p_texture, long p_stage );
    void SetVertexTexture( DrawSpace::Core::Texture* p_texture, long p_stage );
    void SetFx( DrawSpace::Core::Fx* p_fx );
    
    DrawSpace::Core::Texture*   GetTexture( long p_index ) const;
    DrawSpace::Core::Texture*   GetVertexTexture( long p_index ) const;
    DrawSpace::Core::Fx*        GetFx( void ) const;
};
}

