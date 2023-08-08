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

/*
#include "renderingaspectimpl.h"
#include "renderer.h"
#include "plugin.h"
#include "renderingnode.h"
#include "renderpassnodegraph.h"
*/

#include "renderingaspectimpl.h"

/////// fwd decls
namespace DrawSpace
{
namespace Core
{
class RenderingQueue;
}
namespace RenderGraph
{
class RenderPassNodeGraph;
}
namespace EntityGraph
{
class EntityNodeGraph;
}
namespace Utils
{
class TimeManager;
}
}
/////////////////

class MeshRenderingAspectImpl : public DrawSpace::Interface::AspectImplementations::RenderingAspectImpl
{    
protected:
    bool                                m_add_in_rendergraph{ false };
    
public:
    bool VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue );

    void RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );
    void UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );

    bool Init( DrawSpace::Core::Entity* p_entity, DrawSpace::TimeManager* p_timemanager) { return true; };
    virtual void Release(void) {};
    void Run( DrawSpace::Core::Entity* p_entity );
    void SetEntityNodeGraph(DrawSpace::EntityGraph::EntityNodeGraph* p_entitynodegraph) {};
};
