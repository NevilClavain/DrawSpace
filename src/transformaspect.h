/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#ifndef _TRANSFORMASPECT_H_
#define _TRANSFORMASPECT_H_

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
    DrawSpace::Utils::Matrix                                                            m_worldtransform;
    DrawSpace::Utils::Matrix                                                            m_worldtransformfromphysicworld;
    DrawSpace::Utils::Matrix                                                            m_localtransform;

    DrawSpace::Utils::Matrix                                                            m_stack_matrix;

    // transfos view et proj "distribuees" par le worldsystem
    DrawSpace::Utils::Matrix                                                            m_dispatched_viewtransform;
    DrawSpace::Utils::Matrix                                                            m_dispatched_projtransform;

    std::map<int, DrawSpace::Interface::AspectImplementations::TransformAspectImpl*>    m_impls_list;

    TimeAspect*                                                                         m_time_aspect;

public:
    TransformAspect( void );

    void AddImplementation(int p_order, DrawSpace::Interface::AspectImplementations::TransformAspectImpl* p_impl);   
    void RemoveAllImplementations(void);
    
    void GetWorldTransform( DrawSpace::Utils::Matrix& p_worldtransform ) const;
    void GetLocalTransform(DrawSpace::Utils::Matrix& p_localtransform) const;
    void GetWorldTransformFromPhysicWorld( DrawSpace::Utils::Matrix& p_worldtransform ) const;
    void GetStackMatrix(DrawSpace::Utils::Matrix& p_stack) const;

    void GetViewTransform( DrawSpace::Utils::Matrix& p_viewtransform ) const;
    void GetProjTransform( DrawSpace::Utils::Matrix& p_projtransform ) const;
	TimeAspect* GetTimeAspectRef(void) const;

    void ComputeTransforms( Core::Entity* p_parent, Core::Entity* p_entity );
    void DispatchViewProj( const DrawSpace::Utils::Matrix& p_view, DrawSpace::Utils::Matrix& p_proj );

    void SetTimeAspect( TimeAspect* p_time_aspect );

    void OnAddedInGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph, Core::Entity* p_parent_entity);
    void OnRemovedFromGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph, Core::Entity* p_parent_entity);

    std::map<int, DrawSpace::Interface::AspectImplementations::TransformAspectImpl*> GetTransformAspectImplsList(void) const;
};
}
}

#endif
