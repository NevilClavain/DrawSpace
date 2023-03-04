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


#include "drawspace_commons.h"
#include "fx.h"
#include "texture.h"
#include "shader.h"
#include "renderingnode.h"
#include "plugin.h"
#include "renderer.h"
#include "lod_shaderfeeder.h"

namespace LOD
{

// cette classe peut etre vue comme un "descripteur" de renderingNode de planete (SphericalLOD::FaceDrawingNode)
struct Binder
{
private:
    
    DrawSpace::Core::Texture*                                       m_textures[DrawSpace::Core::RenderingNode::NbMaxTextures]; // 32 textures stages max
    DrawSpace::Core::Texture*                                       m_vertextextures[DrawSpace::Core::RenderingNode::NbMaxTextures];

    DrawSpace::Core::Fx*                                            m_fx{ nullptr };

    std::unordered_map<int, ShaderFeeder<DrawSpace::Utils::Vector>> m_vector_shaders_feeders;
    std::unordered_map<int, ShaderFeeder<DrawSpace::Utils::Matrix>> m_matrix_shaders_feeders;

protected:
    DrawSpace::Interface::Renderer*                                 m_renderer{ nullptr };

public:

    Binder( void );

    void BindToShader(void) const;

    void SetRenderer( DrawSpace::Interface::Renderer* p_renderer );

    void SetTexture( DrawSpace::Core::Texture* p_texture, long p_stage );
    void SetVertexTexture( DrawSpace::Core::Texture* p_texture, long p_stage );
    void SetFx( DrawSpace::Core::Fx* p_fx );
    
    DrawSpace::Core::Texture*   GetTexture( long p_index ) const;
    DrawSpace::Core::Texture*   GetVertexTexture( long p_index ) const;
    DrawSpace::Core::Fx*        GetFx( void ) const;
    
    DrawSpace::Utils::Vector    GetShaderFeederValue(DrawSpace::Core::ShaderType p_shader_type, int p_register);

    friend Binder& operator<<(Binder& p_in, const ShaderFeeder<DrawSpace::Utils::Vector>& p_obj);
    friend Binder& operator<<(Binder& p_in, const ShaderFeeder<DrawSpace::Utils::Matrix>& p_obj);
};
}

