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

#include "loadfiletask.h"

using namespace DrawSpace;
using namespace DrawSpace::Systems;

LoadFileTask::LoadFileTask() : Task("", "")
{
}

void LoadFileTask::execute(void)
{
    long size;
    void* data = File::loadAndAllocBinaryFile(m_final_asset_path, &size);
    if (data)
    {
        m_data = data;
        m_size = size;
        m_failure = false;
    }
    else
    {
        m_failure = true;
    }
}

void LoadFileTask::SetFinalAssetPath(const dsstring& p_final_asset_path)
{
    m_final_asset_path = p_final_asset_path;
}

bool LoadFileTask::Failed(void) const
{
    return m_failure;
}

long LoadFileTask::GetSize(void) const
{
    return m_size;
}

void* LoadFileTask::GetData(void) const
{
    return m_data;
}
