/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2020
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

#include "filesystem.h"
#include "memalloc.h"
#include "exceptions.h"


bool DrawSpace::Utils::FileSystem::Exists(const dsstring& p_path)
{
    DWORD dwAttrib = GetFileAttributes(p_path.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES);
}

bool DrawSpace::Utils::FileSystem::IsDirectory(const dsstring& p_path)
{
    DWORD dwAttrib = GetFileAttributes(p_path.c_str());
    return (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

void DrawSpace::Utils::FileSystem::CreateDirectory(const dsstring& p_path)
{
    if(!::CreateDirectory(p_path.c_str(), NULL))
    {
        _DSEXCEPTION("Failed to create directory : " + p_path);
    }
}

long DrawSpace::Utils::FileSystem::FileSize(FILE* p_fp)
{
    long current_pos;
    current_pos = ftell(p_fp);
    fseek(p_fp, 0, SEEK_END);
    long size = ftell(p_fp);
    fseek(p_fp, current_pos, SEEK_SET);
    return size;
}

void* DrawSpace::Utils::FileSystem::LoadAndAllocFile(const dsstring& p_file, long* p_size)
{
    void* ptr = NULL;
    FILE* fp{ fopen(p_file.c_str(), "rb") };
    if (fp)
    {
        unsigned long fs = FileSize(fp);
        ptr = (void*)_DRAWSPACE_NEW_EXPLICIT_SIZE_WITH_COMMENT(unsigned char, unsigned char[fs], fs, p_file);
        if (ptr)
        {
            fread((void*)ptr, fs, 1, fp);
            if (p_size)
            {
                *p_size = fs;
            }
        }
        fclose(fp);
    }
    return ptr;
}

void DrawSpace::Utils::FileSystem::WriteFile(const dsstring& p_file, void* p_data, long p_size)
{
    FILE* fp{ fopen(p_file.c_str(), "wb") };
    if (fp)
    {
        fwrite(p_data, p_size, 1, fp);
        fclose(fp);
    }
}



