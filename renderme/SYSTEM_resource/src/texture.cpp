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

#include "texture.h"

using namespace renderMe;

Texture::Texture(const std::string& p_filename) :
m_source(Source::CONTENT_FROM_FILE),
m_name(p_filename)
{

}

Texture::Texture(const std::string& p_name, Format p_format, size_t p_width, size_t p_height) :
m_name(p_name),
m_source(Source::CONTENT_FROM_RENDERINGQUEUE),
m_format(p_format),
m_width(p_width),
m_height(p_height)
{

}

Texture::Texture(const Texture& p_other)
{
    m_name = p_other.m_name;
	m_source = p_other.m_source;
	m_width = p_other.m_width;
	m_height = p_other.m_height;
	m_format = p_other.m_format;
    m_filecontent = p_other.m_filecontent;

    m_state_mutex.lock();
    p_other.m_state_mutex.lock();
    m_state = p_other.m_state;
    p_other.m_state_mutex.unlock();
    m_state_mutex.unlock();
}

Texture::Source Texture::getSource() const
{
    return m_source;
}

size_t Texture::getWidth() const
{
    return m_width;
}

size_t Texture::getHeight() const
{
    return m_height;
}

Texture::Format Texture::getFormat() const
{
    return m_format;
}

Texture::State Texture::getState() const
{
    m_state_mutex.lock();
    const auto state{ m_state };
    m_state_mutex.unlock();
    return state;
}

void Texture::setState(Texture::State p_state)
{
    m_state_mutex.lock();
    m_state = p_state;
    m_state_mutex.unlock();
}

std::string Texture::getName() const
{
    return m_name;
}