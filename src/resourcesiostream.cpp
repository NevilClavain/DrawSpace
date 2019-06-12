/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2019
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

#include "resourcesiostream.h"

using namespace DrawSpace::Systems;

ResourcesIOStream::ResourcesIOStream(const char* p_strFile, const char* p_strMode) :
m_file(p_strFile, p_strMode)
{
}

ResourcesIOStream::~ResourcesIOStream(void)
{
}

size_t ResourcesIOStream::Read(void* p_buffer, size_t p_size, size_t p_count)
{
    return m_file.Read(p_buffer, p_size, p_count);
}

size_t ResourcesIOStream::Write(const void* p_buffer, size_t p_size, size_t p_count)
{
    return m_file.Write(p_buffer, p_size, p_count);
}

aiReturn ResourcesIOStream::Seek(size_t p_offset, aiOrigin p_origin)
{
    return (m_file.Seek(p_offset, p_origin) ? AI_SUCCESS : AI_FAILURE);
}

size_t ResourcesIOStream::Tell() const
{
    return m_file.Tell();
}

size_t ResourcesIOStream::FileSize() const
{
    return m_file.FileSize();
}

void ResourcesIOStream::Flush()
{
    m_file.Flush();
}
