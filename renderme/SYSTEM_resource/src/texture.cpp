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

#include <md5.h>
#include "texture.h"

using namespace renderMe;

Texture::Texture(Format p_format, size_t p_width, size_t p_height, ContentAccessMode p_content_access_mode) :
m_format(p_format),
m_width(p_width),
m_height(p_height),
m_content_access_mode(p_content_access_mode)
{
}

Texture::Texture(const Texture& p_other)
{
    m_source = p_other.m_source;
    m_source_id = p_other.m_source_id;
    m_resource_uid = p_other.m_resource_uid;
    m_width = p_other.m_width;
    m_height = p_other.m_height;
    m_format = p_other.m_format;
    m_file_content = p_other.m_file_content;
    m_content_access_mode = p_other.m_content_access_mode;

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

int Texture::getWidth() const
{
    return m_width;
}

int Texture::getHeight() const
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

Texture::ContentAccessMode Texture::getContentAccessMode() const
{
    return m_content_access_mode;
}

std::string Texture::getResourceUID() const
{
    return m_resource_uid;
}

void Texture::setState(Texture::State p_state)
{
    m_state_mutex.lock();
    m_state = p_state;
    m_state_mutex.unlock();
}

void Texture::compute_resource_uid()
{
    MD5 md5;

    if (Source::CONTENT_FROM_FILE == m_source)
    {
        const std::string hash_source{ md5.digestMemory((BYTE*)&m_source, (int)(sizeof(m_source))) };
        const std::string hash_source_uid{ md5.digestMemory((BYTE*)&m_source_id, (int)(sizeof(m_source_id))) };
        std::string hash{ hash_source + hash_source_uid };

        m_resource_uid = hash;
    }
    else
    {
        const std::string hash_source{ md5.digestMemory((BYTE*)&m_source, (int)(sizeof(m_source))) };
        const std::string hash_source_uid{ md5.digestMemory((BYTE*)&m_source_id, (int)(sizeof(m_source_id))) };

        const std::string hash_source_width{ md5.digestMemory((BYTE*)&m_width, (int)(sizeof(m_width))) };
        const std::string hash_source_height{ md5.digestMemory((BYTE*)&m_height, (int)(sizeof(m_height))) };
        const std::string hash_source_format{ md5.digestMemory((BYTE*)&m_format, (int)(sizeof(m_format))) };

        const std::string hash_source_content_access_mode{ md5.digestMemory((BYTE*)&m_content_access_mode, (int)(sizeof(m_content_access_mode))) };

        std::string hash{ hash_source + hash_source_uid + hash_source_width + hash_source_height + hash_source_format + hash_source_content_access_mode };
        m_resource_uid = hash;
    }
}

const core::Buffer<unsigned char>& Texture::getFileContent() const
{
    return m_file_content;
}

std::string Texture::getSourceID() const
{
    return m_source_id;
}