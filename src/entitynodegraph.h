/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2017
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

#ifndef _ENTITYNODEGRAPH_H_
#define _ENTITYNODEGRAPH_H_

#include "entitynode.h"

namespace DrawSpace
{
namespace Core
{
class RenderingSystem;
class WorldSystem;
class EntityNodeGraph
{
public:
	using EntityTree = st_tree::tree<Entity*>;

private:
	mutable EntityTree  m_tree;

public:

	EntityNodeGraph(void);
	~EntityNodeGraph(void);

	EntityNode SetRoot(Entity* p_entity);
	void Erase(void);

    void AcceptRenderingSystem( RenderingSystem* p_renderingsystem );
    void AcceptWorldSystem( WorldSystem* p_worldsystem );

};
}
}

#endif