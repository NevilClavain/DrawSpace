
/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include <md5.h>
#include "shader.h"

using namespace mage;

Shader::Shader(int p_type) :
m_type(p_type)
{
}

Shader::Shader(const Shader& p_other)
{
    m_source_id = p_other.m_source_id;
    m_resource_uid = p_other.m_resource_uid;
    m_content = p_other.m_content;
    m_contentSize = p_other.m_contentSize;
    m_code = p_other.m_code;
    m_type = p_other.m_type;

    m_state_mutex.lock();
    p_other.m_state_mutex.lock();
    m_state = p_other.m_state;
    p_other.m_state_mutex.unlock();
    m_state_mutex.unlock();

    m_generic_arguments = p_other.m_generic_arguments;
    m_vectorarray_arguments = p_other.m_vectorarray_arguments;
}

std::string Shader::getContent() const
{
    return m_content;
}

void Shader::setContent(const std::string& p_content)
{
    m_content = p_content;
}

std::string Shader::getResourceUID() const
{
    return m_resource_uid;
}

std::string Shader::getSourceID() const
{
    return m_source_id;
}

size_t Shader::getContentSize() const
{
    return m_contentSize;
}

void Shader::setContentSize(size_t p_contentSize)
{
    m_contentSize = p_contentSize;
}

Shader::State Shader::getState() const
{
    m_state_mutex.lock();
    const auto state{ m_state };
    m_state_mutex.unlock();
    return state;
}

void Shader::setState(State p_state)
{
    m_state_mutex.lock();
    m_state = p_state;
    m_state_mutex.unlock();
}

const core::Buffer<char>& Shader::getCode() const
{
    return m_code;
}

void Shader::setCode(const core::Buffer<char>& p_code)
{
    m_code = p_code;
}

int Shader::getType() const
{
    return m_type;
}

void Shader::addGenericArgument(const GenericArgument& p_arg)
{
    m_generic_arguments.push_back(p_arg);
}

std::vector<Shader::GenericArgument> Shader::getGenericArguments() const
{
    return m_generic_arguments;
}

void Shader::addVectorArrayArgument(const VectorArrayArgument& p_arg)
{
    m_vectorarray_arguments.push_back(p_arg);
}

std::vector<Shader::VectorArrayArgument>& Shader::vectorArrayArgumentsAccess()
{
    return m_vectorarray_arguments;
}

const std::vector<Shader::VectorArrayArgument>& Shader::getVectorArrayArguments() const
{
    return m_vectorarray_arguments;
}

void Shader::compute_resource_uid()
{
    MD5 md5;
    const std::string hash{ md5.digestMemory((BYTE*)m_content.c_str(), m_contentSize)};
    m_resource_uid = hash;
}