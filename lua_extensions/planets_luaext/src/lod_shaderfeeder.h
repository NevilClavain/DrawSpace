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
#include "vector.h"
#include "shader.h"

namespace LOD
{
template<typename T = DrawSpace::Utils::Vector>
struct ShaderFeeder
{
private:
	
    DrawSpace::Core::ShaderType     m_shader{ 0 };
    int							    m_register{ 0 };
	T	                            m_value;
   
public:

    ShaderFeeder(void) { };
    ShaderFeeder(DrawSpace::Core::ShaderType p_shader_type, int p_register, const T& p_value) :
        m_shader(p_shader_type),
        m_register(p_register),
        m_value(p_value)
    {
    }

    inline int Hash(void) const
    {
        return LOD::ComputeHash(m_shader, m_register);
    }

    inline int GetShaderType(void) const
    {
        return static_cast<int>(m_shader);
    }

    inline int GetRegister(void) const
    {
        return m_register;
    }

    inline T GetValue(void) const
    {
        return m_value;
    }
};

int ComputeHash(DrawSpace::Core::ShaderType p_shader_type, int p_register);

}
