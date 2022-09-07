
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
#include "vector.h"

namespace LOD
{
// fwd decl 
struct Binder;

struct ShaderFeeder
{
public:
    enum class ShaderType
    {
        VERTEX_SHADER   = 0,
        PIXEL_SHADER    = 1
    };

private:
	
    ShaderType                  m_shader{ 0 };
    int							m_register{ 0 };
	DrawSpace::Utils::Vector	m_value;
   
public:

    ShaderFeeder(void) { };
    ShaderFeeder(ShaderType p_shader_type, int p_register, const DrawSpace::Utils::Vector& p_value);

    static int ComputeHash(ShaderType p_shader_type, int p_register);

    int Hash(void) const;

    int                         GetShaderType(void) const;
    int                         GetRegister(void) const;
    DrawSpace::Utils::Vector    GetValue(void) const;
};

}
