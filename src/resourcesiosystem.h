#pragma once

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

#include <assimp\IOSystem.hpp>

namespace DrawSpace
{
namespace Systems
{
class ResourcesIOStream;

class ResourcesIOSystem : public Assimp::IOSystem
{

public:
    ResourcesIOSystem(void);
    ~ResourcesIOSystem(void);

    bool Exists(const char* p_file) const;
    
    char getOsSeparator() const;

    Assimp::IOStream* Open(const char* p_file, const char* p_mode = "rb");
    void Close(Assimp::IOStream* p_file);
};
}
}
