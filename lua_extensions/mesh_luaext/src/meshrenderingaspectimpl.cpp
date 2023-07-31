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

#include "meshrenderingaspectimpl.h"
#include "passslot.h"
#include "renderingaspect.h"
#include "renderpassnodegraph.h"
#include "transformaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::RenderGraph;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Maths;

bool MeshRenderingAspectImpl::VisitRenderPassDescr( const dsstring& p_name, RenderingQueue* p_passqueue )
{
    bool updated_queue{ false };

    ComponentList<PassSlot> pass_slots;

    m_owner->GetComponentsByType<PassSlot>( pass_slots );

    for( size_t i = 0; i < pass_slots.size(); i++ )
    {
        if( pass_slots.at(i)->getPurpose().GetPassName() == p_name )
        {
            const auto rnode{ pass_slots.at(i)->getPurpose().GetRenderingNode() };
            if( m_add_in_rendergraph )
            {
                rnode->m_debug_id = "meshe_node";
                // ajout du renderingnode dans la renderingqueue  
                p_passqueue->Add( rnode );
            }
            else
            {
                // suppression du renderingnode de la renderingqueue
                p_passqueue->Remove( rnode );
            }
            updated_queue = true;
        }
    }
    return updated_queue;
}

void MeshRenderingAspectImpl::RegisterToRendering( RenderPassNodeGraph& p_rendergraph )
{
    m_add_in_rendergraph = true;
    p_rendergraph.Accept( this );
}

void MeshRenderingAspectImpl::UnregisterFromRendering( RenderPassNodeGraph& p_rendergraph )
{
    m_add_in_rendergraph = false;
    p_rendergraph.Accept( this );
}

void MeshRenderingAspectImpl::Run( Entity* p_entity )
{
    const auto transform_aspect{ p_entity->GetAspect<TransformAspect>() };

    if( transform_aspect )
    {
        Matrix world;
        transform_aspect->GetWorldTransform( world );

        Matrix view;
        transform_aspect->GetViewTransform( view );

        Matrix proj;
        transform_aspect->GetProjTransform( proj );

        // redistribution de la transfo world...
        // + redistribution des proj/view distribues

        ComponentList<PassSlot> pass_slots;
        m_owner->GetComponentsByType<PassSlot>( pass_slots );

        for( size_t i = 0; i < pass_slots.size(); i++ )
        {
            pass_slots.at(i)->getPurpose().m_world = world;
            pass_slots.at(i)->getPurpose().m_view = view;
            pass_slots.at(i)->getPurpose().m_proj = proj;
        }
    }
}
