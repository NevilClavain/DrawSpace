
/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2019
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

#include "resourcesiosystem.h"
#include "resourcesiostream.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Systems;


ResourcesIOSystem::ResourcesIOSystem(void)
{
}

ResourcesIOSystem::~ResourcesIOSystem(void)
{
}

//most of the code here is extracted from Assimp::DefaultIOSystem class

bool ResourcesIOSystem::Exists(const char* p_file) const
{
    FILE* file = ::fopen(p_file, "rb");
    if (!file)
        return false;

    ::fclose(file);
    return true;
}

char ResourcesIOSystem::getOsSeparator() const
{
#ifndef _WIN32
    return '/';
#else
    return '\\';
#endif
}

Assimp::IOStream* ResourcesIOSystem::Open(const char* p_file, const char* p_mode)
{
    return _DRAWSPACE_NEW_(ResourcesIOStream, ResourcesIOStream(p_file, p_mode));
}

void ResourcesIOSystem::Close(Assimp::IOStream* p_file)
{
    _DRAWSPACE_DELETE_( p_file );
}