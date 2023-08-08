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

#include "readshadermd5task.h"
#include "memalloc.h"
#include "filesystem.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Systems;

const dsstring ReadShaderMD5Task::bcCacheName{ "bc_cache" };
const dsstring ReadShaderMD5Task::bcMd5FileName{ "bc.md5" };


ReadShaderMD5Task::ReadShaderMD5Task() : ITask("READMD5SHADERFILE", "")
{
}

void ReadShaderMD5Task::Execute(void)
{
    m_failure = false;

    dsstring path{ bcCacheName + dsstring("/") + m_shader_id.c_str() };

    long md5filesize;
    unsigned char* md5Buf = { static_cast<unsigned char*>(FileSystem::loadAndAllocFile(path + dsstring("\\") + bcMd5FileName, &md5filesize)) };

    if (md5Buf)
    {
        dsstring stored_md5((char*)md5Buf, md5filesize);
        _DRAWSPACE_DELETE_N_(md5Buf);
        m_loaded_md5 = stored_md5;

    }
    else
    {
        m_failure = true;
    }
}

void ReadShaderMD5Task::SetShaderId(const dsstring& p_shader_id)
{
    m_shader_id = p_shader_id;
}

void ReadShaderMD5Task::SetFilePath(const dsstring& p_filepath)
{
    m_filepath = p_filepath;
}

void ReadShaderMD5Task::SetCompareMD5(const dsstring& p_md5)
{
    m_compare_md5 = p_md5;
}

bool ReadShaderMD5Task::MD5AreEquals(void) const
{
    return (m_compare_md5 == m_loaded_md5);
}

bool ReadShaderMD5Task::Failed(void) const
{
    return m_failure;
}
