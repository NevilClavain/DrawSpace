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

#include "lod_binder.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace LOD;

Binder::Binder( void )
{
    for( long i = 0; i < RenderingNode::GetTextureListSize(); i++ )
    {
        m_textures[i] = nullptr;
        m_vertextextures[i] = nullptr;
    }
}

void Binder::SetRenderer( DrawSpace::Interface::Renderer* p_renderer )
{
    m_renderer = p_renderer;
}

void Binder::SetTexture( Core::Texture* p_texture, long p_stage )
{
    m_textures[p_stage] = p_texture;
}

void Binder::SetVertexTexture( Core::Texture* p_texture, long p_stage )
{
    m_vertextextures[p_stage] = p_texture;
}

void Binder::SetFx( Core::Fx* p_fx )
{
    m_fx = p_fx;
}
    
Texture* Binder::GetTexture( long p_index ) const
{
    return m_textures[p_index];
}

Texture* Binder::GetVertexTexture( long p_index ) const
{
    return m_vertextextures[p_index];
}

Fx* Binder::GetFx( void ) const
{
    return m_fx;
}

void Binder::BindToShader(void) const
{
    for (auto& e : m_shaders_feeders)
    {
        m_renderer->SetFxShaderParams(e.second.GetShaderType(), e.second.GetRegister(), e.second.GetValue());
    }
}

DrawSpace::Utils::Vector Binder::GetShaderFeederValue(ShaderFeeder::ShaderType p_shader_type, int p_register)
{
    const int key{ ShaderFeeder::ComputeHash(p_shader_type, p_register) };
    if (m_shaders_feeders.count(key))
    {
        return m_shaders_feeders.at(key).GetValue();
    }
    else
    {
        _DSEXCEPTION("unknow shader feeder");
    }
}

Binder& LOD::operator<<(Binder& p_in, const ShaderFeeder& p_obj)
{
    p_in.m_shaders_feeders[p_obj.Hash()] = p_obj;
    return p_in;
}