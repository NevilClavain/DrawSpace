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


namespace DrawSpace
{
namespace Interface   
{
class Renderer;
}

namespace Systems
{
struct CompileShaderTask : public Interface::ITask
{
private:

    bool                            m_failure;
    dsstring                        m_err_compil;

    DrawSpace::Interface::Renderer* m_renderer{ nullptr };

    long                            m_text_size;
    void*                           m_text{ nullptr };
    int                             m_shadertype;

    dsstring                        m_final_asset_dir;
    dsstring                        m_asset_path;

    // shader bytecode buffer infos...
    long                            m_bc_length;
    void* m_bc{ nullptr };

public:

    CompileShaderTask();

    void Execute(void);
    void SetShaderText(void* p_text, long p_text_size, int p_shadertype);
    void SetShaderDirs(const dsstring& p_final_asset_dir, const dsstring& p_asset_path);
    void SetRenderer(DrawSpace::Interface::Renderer* p_renderer);
    bool Failed(void) const;
    dsstring GetCompilationError(void) const;
    void* GetShaderByteCode(void) const;
    long GetShaderByteCodeLength(void) const;
};
}
}

