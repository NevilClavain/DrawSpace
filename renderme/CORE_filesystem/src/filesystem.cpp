/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include <windows.h>
#include "filesystem.h"
#include "exceptions.h"

using namespace renderMe::core;

bool FileSystem::exists(const std::string& p_path)
{
    const auto dwAttrib{ GetFileAttributes(p_path.c_str()) };
    return (dwAttrib != INVALID_FILE_ATTRIBUTES);
}

bool FileSystem::isDirectory(const std::string& p_path)
{
    const auto dwAttrib{ GetFileAttributes(p_path.c_str()) };
    return (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

void FileSystem::createDirectory(const std::string& p_path)
{
    if(!::CreateDirectory(p_path.c_str(), nullptr))
    {
        _EXCEPTION("Failed to create directory : " + p_path);
    }
}

long FileSystem::fileSize(FILE* p_fp)
{  
    const auto current_pos{ ftell(p_fp) };
    fseek(p_fp, 0, SEEK_END);
    const auto size{ ftell(p_fp) };
    fseek(p_fp, current_pos, SEEK_SET);
    return size;
}





