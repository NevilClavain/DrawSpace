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

#include "transformaspectimpl.h"
#include "timemanager.h"
#include "entity.h"



namespace DrawSpace
{
namespace Aspect
{
class TimeAspect;
class TransformAspect : public Core::ComponentContainer
{
protected:
    DrawSpace::Maths::Matrix                                                            m_worldtransform;
    DrawSpace::Maths::Matrix                                                            m_worldtransformfromphysicworld;
    DrawSpace::Maths::Matrix                                                            m_localtransform;

    DrawSpace::Maths::Matrix                                                            m_stack_matrix;

    // transfos view et proj "distribuees" par le worldsystem
    DrawSpace::Maths::Matrix                                                            m_dispatched_viewtransform;
    DrawSpace::Maths::Matrix                                                            m_dispatched_projtransform;

    std::map<int, DrawSpace::Interface::AspectImplementations::TransformAspectImpl*>    m_impls_list;

    TimeAspect*                                                                         m_time_aspect;

public:
    TransformAspect( void );

    void AddImplementation(int p_order, DrawSpace::Interface::AspectImplementations::TransformAspectImpl* p_impl);   
    void RemoveAllImplementations(void);
    
    void GetWorldTransform( DrawSpace::Maths::Matrix& p_worldtransform ) const;
    void GetLocalTransform(DrawSpace::Maths::Matrix& p_localtransform) const;
    void GetWorldTransformFromPhysicWorld( DrawSpace::Maths::Matrix& p_worldtransform ) const;
    void GetStackMatrix(DrawSpace::Maths::Matrix& p_stack) const;

    void GetViewTransform( DrawSpace::Maths::Matrix& p_viewtransform ) const;
    void GetProjTransform( DrawSpace::Maths::Matrix& p_projtransform ) const;
	TimeAspect* GetTimeAspectRef(void) const;

    void ComputeTransforms( Core::Entity* p_parent, Core::Entity* p_entity );
    void DispatchViewProj( const DrawSpace::Maths::Matrix& p_view, DrawSpace::Maths::Matrix& p_proj );

    void SetTimeAspect( TimeAspect* p_time_aspect );

    void OnAddedInGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph, Core::Entity* p_parent_entity);
    void OnRemovedFromGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph, Core::Entity* p_parent_entity);


    bool ProjectLocalPoint(const DrawSpace::Utils::Vector& p_local_point, dsreal& p_posx, dsreal& p_posy);

    dsreal LocalPointDistanceFromCamera(const DrawSpace::Utils::Vector& p_local_point);

    std::map<int, DrawSpace::Interface::AspectImplementations::TransformAspectImpl*> GetTransformAspectImplsList(void) const;
};
}
}

