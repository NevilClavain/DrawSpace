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

#include "lod_shaderfeeder.h"
#include "lod_binder.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;
using namespace LOD;

ShaderFeeder::ShaderFeeder(ShaderType p_shader_type, int p_register, const DrawSpace::Utils::Vector& p_value) :
	m_shader(p_shader_type),
	m_register(p_register),
	m_value(p_value)
{
}

int ShaderFeeder::ComputeHash(ShaderType p_shader_type, int p_register)
{
	std::hash<int> hash;
	return hash((static_cast<int>(p_shader_type) * 1000) + p_register);
}

int ShaderFeeder::Hash(void) const
{
	return ShaderFeeder::ComputeHash(m_shader, m_register);
}

int ShaderFeeder::GetShaderType(void) const
{
	return static_cast<int>(m_shader);
}

int ShaderFeeder::GetRegister(void) const
{
	return m_register;
}

DrawSpace::Utils::Vector ShaderFeeder::GetValue(void) const
{
	return m_value;
}