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

#include "createdirectorytask.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Systems;

const dsstring CreateDirectoryTask::bcCacheName{ "bc_cache" };

CreateDirectoryTask::CreateDirectoryTask() : ITask("CREATEDIRECTORY", "")
{
}

void CreateDirectoryTask::Execute(void)
{
    dsstring path{ bcCacheName + dsstring("\\") + m_shader_id.c_str() };
    DrawSpace::Utils::FileSystem::CreateDirectory(path);
}

void CreateDirectoryTask::SetShaderId(const dsstring& p_shader_id)
{
    m_shader_id = p_shader_id;
}
