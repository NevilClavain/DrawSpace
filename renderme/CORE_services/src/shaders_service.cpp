/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include "shaders_service.h"

using namespace renderMe::core::services;

void ShadersCompilationService::requestVertexCompilationShader(const std::string& p_includePath, 
																const renderMe::core::FileContent<const char>& p_shaderSource, 
																std::unique_ptr<char[]>& shaderBytes, size_t& shaderBytesLength,
																bool& p_status) const
{
	if (m_callbacks.size() > 0)
	{
		const auto func{ m_callbacks.at(0) };
		func(p_includePath, p_shaderSource, 0, shaderBytes, shaderBytesLength, p_status);
	}
}

void ShadersCompilationService::requestPixelCompilationShader(const std::string& p_includePath, 
																const renderMe::core::FileContent<const char>& p_shaderSource, 
																std::unique_ptr<char[]>& shaderBytes, size_t& shaderBytesLength,
																bool& p_status) const
{
	if (m_callbacks.size() > 0)
	{
		const auto func{ m_callbacks.at(0) };
		func(p_includePath, p_shaderSource, 1, shaderBytes, shaderBytesLength, p_status);
	}
}
