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

#include "transformaspect.h"

#include "physicsaspect.h"
#include "timeaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

TransformAspect::TransformAspect( void ) :
m_time_aspect( NULL )
{
    m_worldtransform.Identity();
    m_worldtransformfromphysicworld.Identity();
    m_localtransform.Identity();

    m_dispatched_viewtransform.Identity();
    m_dispatched_projtransform.Identity();
    m_stack_matrix.Identity();
}

void TransformAspect::AddImplementation(int p_order, DrawSpace::Interface::AspectImplementations::TransformAspectImpl* p_impl)
{
    if (m_time_aspect)
    {
        p_impl->SetTimeAspect(m_time_aspect);
    }

    m_impls_list[p_order] = p_impl;
}

void TransformAspect::RemoveAllImplementations(void)
{
    for (auto& e : m_impls_list)
    {
        e.second->SetTimeAspect(nullptr);
    }
    m_impls_list.clear();
}


void TransformAspect::ComputeTransforms( Entity* p_parent, Entity* p_entity )
{
    Matrix locale_mat;

    locale_mat.Identity();

    // operation : pour une liste de N matrices
    // [N] * [N-1] * [N - 2] * ..... * [2] * [1] * [0]

    // la 1ERE matrice appliquee est la matrice N

    for (auto& e : m_impls_list)
    {
        Matrix res, current;

        e.second->GetLocaleTransform(this, current);

        Matrix::MatrixMult(&current, &locale_mat, &res);
        locale_mat = res;
    }

    m_localtransform = locale_mat;

    Matrix parent_transform_mat;
    parent_transform_mat.Identity();

    Matrix parent_transform_mat_fromphysicworld;
    parent_transform_mat_fromphysicworld.Identity();

    if( p_parent )
    {
        TransformAspect* parent_world_aspect = p_parent->GetAspect<TransformAspect>();
        if (parent_world_aspect)
        {
            parent_transform_mat = parent_world_aspect->m_worldtransform;
            parent_transform_mat_fromphysicworld = parent_world_aspect->m_worldtransformfromphysicworld;
        }        
    }
    
    Matrix sp = m_stack_matrix * parent_transform_mat;
    m_worldtransform = locale_mat * sp;


    PhysicsAspect* physics_aspect{ p_entity->GetAspect<PhysicsAspect>() };
    if (physics_aspect)
    {
        m_worldtransformfromphysicworld.Identity();
    }
    else
    {
        Matrix sp2 = m_stack_matrix * parent_transform_mat_fromphysicworld;
        m_worldtransformfromphysicworld = locale_mat * sp2;
    }
}

void TransformAspect::DispatchViewProj( const DrawSpace::Utils::Matrix& p_view, DrawSpace::Utils::Matrix& p_proj )
{
    m_dispatched_viewtransform = p_view;
    m_dispatched_projtransform = p_proj;
}

void TransformAspect::GetWorldTransform( Matrix& p_worldtransform ) const
{
    p_worldtransform = m_worldtransform;
}

void TransformAspect::GetLocalTransform(DrawSpace::Utils::Matrix& p_localtransform) const
{
    p_localtransform = m_localtransform;
}

void TransformAspect::GetWorldTransformFromPhysicWorld(Matrix& p_worldtransform) const
{
    p_worldtransform = m_worldtransformfromphysicworld;
}

void TransformAspect::GetStackMatrix(DrawSpace::Utils::Matrix& p_stack) const
{
    p_stack = m_stack_matrix;
}

void TransformAspect::GetViewTransform( DrawSpace::Utils::Matrix& p_viewtransform ) const
{
    p_viewtransform = m_dispatched_viewtransform;
}

void TransformAspect::GetProjTransform( DrawSpace::Utils::Matrix& p_projtransform ) const
{
    p_projtransform = m_dispatched_projtransform;
}

TimeAspect* TransformAspect::GetTimeAspectRef(void) const
{
	return m_time_aspect;
}

std::map<int, DrawSpace::Interface::AspectImplementations::TransformAspectImpl*> TransformAspect::GetTransformAspectImplsList(void) const
{
    return m_impls_list;
}

void TransformAspect::SetTimeAspect( TimeAspect* p_time_aspect )
{
    m_time_aspect = p_time_aspect;
   
    for (auto& e : m_impls_list)
    {
        e.second->SetTimeAspect(p_time_aspect);
    }
}

void TransformAspect::OnAddedInGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph, Entity* p_parent_entity)
{
    Matrix parent_transform_mat;
    parent_transform_mat.Identity();

    TransformAspect* parent_transform_aspect{ p_parent_entity->GetAspect<TransformAspect>() };

    if (parent_transform_aspect)
    {
        Matrix parent_transform{ parent_transform_aspect->m_worldtransform };

        parent_transform_mat = parent_transform;
       
        parent_transform.Inverse();

        Matrix current_stack_matrix{ m_stack_matrix };
        m_stack_matrix = current_stack_matrix * parent_transform;
    }

    for (auto& e : m_impls_list)
    {
        e.second->OnAddedInGraph(this, parent_transform_mat);
    }

}

void TransformAspect::OnRemovedFromGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph, Entity* p_parent_entity)
{
    Matrix parent_transform_mat;
    parent_transform_mat.Identity();

    if (p_parent_entity)
    {
        TransformAspect* parent_transform_aspect{ p_parent_entity->GetAspect<TransformAspect>() };
        if (parent_transform_aspect)
        {
            Matrix parent_transform{ parent_transform_aspect->m_worldtransform };
            parent_transform_mat = parent_transform;
        }
    }

    for (auto& e : m_impls_list)
    {
        e.second->OnRemovedFromGraph(this, parent_transform_mat);
    }

    if (p_parent_entity)
    {
        TransformAspect* parent_transform_aspect{ p_parent_entity->GetAspect<TransformAspect>() };
        if (parent_transform_aspect)
        {
            Matrix parent_transform{ parent_transform_aspect->m_worldtransform };
            parent_transform_mat = parent_transform;

            Matrix current_stack_matrix{ m_stack_matrix };
            m_stack_matrix = current_stack_matrix * parent_transform;
        }
    }   
}
