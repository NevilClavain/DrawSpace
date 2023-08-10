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

#include "compileshadertask.h"
#include "memalloc.h"
#include "renderer.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Systems;

CompileShaderTask::CompileShaderTask() : Task("COMPILESHADER", "")
{
}

void CompileShaderTask::execute(void)
{
    m_failure = false;

    void* bytecode_handle;

    bool comp_status{ m_renderer->CreateShaderBytes((char*)m_text, m_text_size, m_shadertype, m_asset_path, m_final_asset_dir, &bytecode_handle) };
    if (!comp_status)
    {
        dsstring err_compil{ m_renderer->GetShaderCompilationError(bytecode_handle) };

        m_failure = true;
        m_err_compil = err_compil;
    }
    else
    {
        m_bc_length = m_renderer->GetShaderBytesLength(bytecode_handle);

        m_bc = (void*)_DRAWSPACE_NEW_EXPLICIT_SIZE_WITH_COMMENT(unsigned char, unsigned char[m_bc_length], m_bc_length, m_final_asset_dir);
        memcpy(m_bc, m_renderer->GetShaderBytes(bytecode_handle), m_bc_length);

        m_renderer->ReleaseShaderBytes(bytecode_handle);
    }
}

void CompileShaderTask::SetShaderText(void* p_text, long p_text_size, int p_shadertype)
{
    m_text_size = p_text_size;
    m_text = p_text;
    m_shadertype = p_shadertype;
}

void CompileShaderTask::SetShaderDirs(const dsstring& p_final_asset_dir, const dsstring& p_asset_path)
{
    m_final_asset_dir = p_final_asset_dir;
    m_asset_path = p_asset_path;
}

void CompileShaderTask::SetRenderer(DrawSpace::Interface::Renderer* p_renderer)
{
    m_renderer = p_renderer;
}

bool CompileShaderTask::CompileShaderTask::Failed(void) const
{
    return m_failure;
}

dsstring CompileShaderTask::GetCompilationError(void) const
{
    return m_err_compil;
}

void* CompileShaderTask::GetShaderByteCode(void) const
{
    return m_bc;
}

long CompileShaderTask::GetShaderByteCodeLength(void) const
{
    return m_bc_length;
}
