
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

#include "shader.h"

using namespace renderMe;

Shader::Shader(const std::string& p_name, int p_type) :
    m_name(p_name),
    m_type(p_type)
{
}

Shader::Shader(const Shader& p_other)
{
    m_name = p_other.m_name;
    m_content = p_other.m_content;
    m_contentMD5 = p_other.m_contentMD5;
    m_contentSize = p_other.m_contentSize;
    m_code = p_other.m_code;
    m_type = p_other.m_type;

    m_state_mutex.lock();
    p_other.m_state_mutex.lock();
    m_state = p_other.m_state;
    p_other.m_state_mutex.unlock();
    m_state_mutex.unlock();
}

std::string Shader::getName() const
{
    return m_name;
}

std::string Shader::getContent() const
{
    return m_content;
}

void Shader::setContent(const std::string& p_content)
{
    m_content = p_content;
}

std::string Shader::getContentMD5() const
{
    return m_contentMD5;
}

void Shader::setContentMD5(const std::string& p_contentMD5)
{
    m_contentMD5 = p_contentMD5;
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

void Shader::addArgument(const Argument& p_arg)
{
    m_arguments.push_back(p_arg);
}