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

#pragma once

#include "task.h"
#include "file.h"
#include "filesystem.h"

namespace DrawSpace
{
namespace Systems
{
struct LoadShaderFileTask : public Interface::ITask
{
private:

    dsstring            m_final_asset_path;
    dsstring            m_final_asset_dir;

    long                m_size{ 0 };
    void*               m_data{ nullptr };
    bool                m_failure{ false };

    std::vector<void*>  m_blocs;

    void split_source(dsstring& p_source, std::list<dsstring>& p_decomposed_source, std::list<dsstring>& p_decomposed_source_whitecars);
    void recurs_browse_shader_source(const dsstring& p_filepath, dsstring& p_updated_source);

    void release_blocs(void);

public:

    LoadShaderFileTask();
    ~LoadShaderFileTask();

    void Execute(void);

    void SetFinalAssetPath(const dsstring& p_final_asset_path);
    void SetFinalAssetDir(const dsstring& p_final_asset_dir);
    bool Failed(void) const;
    long GetSize(void) const;
    void* GetData(void) const;
};
}
}
