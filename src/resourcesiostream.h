#pragma once

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

#include <assimp\IOStream.hpp>
#include "drawspace_commons.h"
#include "file.h"

namespace DrawSpace
{
namespace Systems
{
class ResourcesIOStream : public Assimp::IOStream
{
private:

    Utils::File m_file;

public:
    ResourcesIOStream(const char* p_strFile, const char* p_strMode);
    ~ResourcesIOStream(void);

    size_t Read(void* p_buffer, size_t p_size, size_t p_count);
    size_t Write(const void* p_buffer, size_t p_size, size_t p_count);

    aiReturn Seek(size_t p_offset, aiOrigin p_origin);
    size_t Tell() const;
    size_t FileSize() const;
    void Flush();
};
}
}

