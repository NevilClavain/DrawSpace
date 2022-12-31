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

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "task.h"
#include "file.h"
#include "filesystem.h"

namespace DrawSpace
{
namespace Core
{
    class Meshe;
}
namespace Systems
{
    struct LoadFileToAssimpTask : public Interface::ITask
    {
    private:

        bool                                m_failure;
        dsstring                            m_err_descr;

        dsstring                            m_final_asset_path;

        DrawSpace::Core::Meshe* m_target_meshe;
        const aiScene* m_scene{ nullptr };
        Assimp::Importer* m_importer{ nullptr };

    public:

        LoadFileToAssimpTask();

        void Execute(void);
        void SetFinalAssetPath(const dsstring& p_final_asset_path);

        void SetTargetMeshe(DrawSpace::Core::Meshe* p_meshe);

        bool Failed(void) const;

        const aiScene* GetScene(void) const;

        Assimp::Importer* GetImporter(void) const;
    };
}
}
