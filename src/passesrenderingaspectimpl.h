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

#include "renderingaspectimpl.h"
#include "renderpassnodegraph.h"

namespace DrawSpace
{
namespace AspectImplementations
{
class PassesRenderingAspectImpl : public DrawSpace::Interface::AspectImplementations::RenderingAspectImpl
{
protected:
    DrawSpace::RenderGraph::RenderPassNodeGraph* m_rendergraph;

public:

    PassesRenderingAspectImpl( void );
    bool VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue );
    void SetRendergraph( DrawSpace::RenderGraph::RenderPassNodeGraph* p_rendergraph );
    bool Init( DrawSpace::Core::Entity* p_entity, DrawSpace::Utils::TimeManager* p_timemanager) { return true; };
    void Release(void) {};
    void Run( DrawSpace::Core::Entity* p_entity );
    void SetEntityNodeGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph) {};
};
}
}

