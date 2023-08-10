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
namespace Systems
{

struct ReadShaderMD5Task : public Task
{
private:

    static const dsstring bcCacheName;
    static const dsstring bcMd5FileName;

    // execution data
    dsstring        m_shader_id;
    dsstring        m_filepath;

    bool            m_failure;

    dsstring        m_compare_md5;

    dsstring        m_loaded_md5;

    bool            m_md5_are_equals;

public:

    ReadShaderMD5Task();

    void execute(void);

    void SetShaderId(const dsstring& p_shader_id);
    void SetFilePath(const dsstring& p_filepath);
    void SetCompareMD5(const dsstring& p_md5);
    bool MD5AreEquals(void) const;
    bool Failed(void) const;

};

}
}
