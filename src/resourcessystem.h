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

#include "systems.h"
#include "entitynodegraph.h"

namespace DrawSpace
{
namespace Systems
{
class ResourcesSystem : public Interface::System
{
protected:

    using Bloc = struct
    {
        void*   data;
        int     size;
    };

    static dsstring                 m_textures_rootpath;

    std::map<dsstring,Bloc>         m_texturesBloc;





    void run(EntityGraph::EntityNodeGraph* p_entitygraph);

    dsstring compute_textures_final_path(const dsstring& p_path) const;

public:
    dsstring GetSystemId(void) const { return "ResourcesSystem"; };

    void VisitEntity(Core::Entity* p_parent, Core::Entity* p_entity);


    void SetTexturesRootPath(const dsstring& p_path);

};
}
}

