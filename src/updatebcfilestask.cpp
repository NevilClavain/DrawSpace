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

#include "updatebcfilestask.h"
#include "filesystem.h"


using namespace DrawSpace;
using namespace DrawSpace::Systems;

const dsstring UpdateBCFilesTask::bcCacheName{ "bc_cache" };
const dsstring UpdateBCFilesTask::bcMd5FileName{ "bc.md5" };
const dsstring UpdateBCFilesTask::bcCodeFileName{ "bc.code" };
const dsstring UpdateBCFilesTask::sourceFileNameExt{ ".hlsl" };


UpdateBCFilesTask::UpdateBCFilesTask() : ITask("UPDATEBCFILES", "")
{
}

void UpdateBCFilesTask::SetShaderText(void* p_text, long p_text_size)
{
    m_text_size = p_text_size;
    m_text = p_text;
}

void UpdateBCFilesTask::Execute(void)
{
    dsstring path{ bcCacheName + dsstring("/") + m_shader_id.c_str() };

    DrawSpace::Utils::FileSystem::WriteFile(path + dsstring("\\") + bcMd5FileName, (void*)m_hash.c_str(), m_hash.length());
    DrawSpace::Utils::FileSystem::WriteFile(path + dsstring("\\") + bcCodeFileName, m_bc, m_bc_length);


    DrawSpace::Utils::FileSystem::WriteFile(path + dsstring("\\") + m_shader_id + sourceFileNameExt, m_text, m_text_size);

}

void UpdateBCFilesTask::UpdateBCFilesTask::SetBC(void* p_bc, long p_bc_length)
{
    m_bc = p_bc;
    m_bc_length = p_bc_length;
}

void UpdateBCFilesTask::SetHash(const dsstring& p_hash)
{
    m_hash = p_hash;
}

void UpdateBCFilesTask::SetShaderId(const dsstring& p_shader_id)
{
    m_shader_id = p_shader_id;
}
