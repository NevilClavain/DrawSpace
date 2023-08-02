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

#include "transformaspect.h"

#include "physicsaspect.h"
#include "timeaspect.h"

#include "transformation.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Maths;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

TransformAspect::TransformAspect( void ) :
m_time_aspect( NULL )
{
    m_worldtransform.identity();
    m_worldtransformfromphysicworld.identity();
    m_localtransform.identity();

    m_dispatched_viewtransform.identity();
    m_dispatched_projtransform.identity();
    m_stack_matrix.identity();
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

    locale_mat.identity();

    // operation : pour une liste de N matrices
    // [N] * [N-1] * [N - 2] * ..... * [2] * [1] * [0]

    // la 1ERE matrice appliquee est la matrice N

    for (auto& e : m_impls_list)
    {
        Matrix res, current;

        e.second->GetLocaleTransform(this, current);

        Matrix::matrixMult(&current, &locale_mat, &res);
        locale_mat = res;
    }

    Matrix parent_transform_mat;
    parent_transform_mat.identity();

    Matrix parent_transform_mat_fromphysicworld;
    parent_transform_mat_fromphysicworld.identity();

    if( p_parent )
    {
        TransformAspect* parent_world_aspect = p_parent->GetAspect<TransformAspect>();
        if (parent_world_aspect)
        {
            parent_transform_mat = parent_world_aspect->m_worldtransform;
            parent_transform_mat_fromphysicworld = parent_world_aspect->m_worldtransformfromphysicworld;
        }        
    }

    // la "vraie" transformation locale n'est pas locale_mat seule mais locale_mat * m_stack_matrix -> m_stack_matrix utlis�e pour empiler les transfo des "attach/detach" successifs
    m_localtransform = locale_mat * m_stack_matrix;
    
    Matrix sp = m_stack_matrix * parent_transform_mat;
    m_worldtransform = locale_mat * sp;


    PhysicsAspect* physics_aspect{ p_entity->GetAspect<PhysicsAspect>() };
    if (physics_aspect)
    {
        m_worldtransformfromphysicworld.identity();
    }
    else
    {
        Matrix sp2 = m_stack_matrix * parent_transform_mat_fromphysicworld;
        m_worldtransformfromphysicworld = locale_mat * sp2;
    }
}

void TransformAspect::DispatchViewProj( const DrawSpace::Maths::Matrix& p_view, DrawSpace::Maths::Matrix& p_proj )
{
    m_dispatched_viewtransform = p_view;
    m_dispatched_projtransform = p_proj;
}

void TransformAspect::GetWorldTransform( Matrix& p_worldtransform ) const
{
    p_worldtransform = m_worldtransform;
}

void TransformAspect::GetLocalTransform(DrawSpace::Maths::Matrix& p_localtransform) const
{
    p_localtransform = m_localtransform;
}

void TransformAspect::GetWorldTransformFromPhysicWorld(Matrix& p_worldtransform) const
{
    p_worldtransform = m_worldtransformfromphysicworld;
}

void TransformAspect::GetStackMatrix(DrawSpace::Maths::Matrix& p_stack) const
{
    p_stack = m_stack_matrix;
}

void TransformAspect::GetViewTransform( DrawSpace::Maths::Matrix& p_viewtransform ) const
{
    p_viewtransform = m_dispatched_viewtransform;
}

void TransformAspect::GetProjTransform( DrawSpace::Maths::Matrix& p_projtransform ) const
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
    parent_transform_mat.identity();

    TransformAspect* parent_transform_aspect{ p_parent_entity->GetAspect<TransformAspect>() };

    if (parent_transform_aspect)
    {
        Matrix parent_transform{ parent_transform_aspect->m_worldtransform };

        parent_transform_mat = parent_transform;
       
        parent_transform.inverse();

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
    parent_transform_mat.identity();

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

bool TransformAspect::ProjectLocalPoint(const DrawSpace::Maths::Vector& p_local_point, dsreal& p_posx, dsreal& p_posy)
{
    
    DrawSpace::Maths::Matrix final_view;
    DrawSpace::Maths::Matrix inv;
    inv.identity();
    inv(2, 2) = -1.0;
    final_view = m_dispatched_viewtransform * inv;

    DrawSpace::Maths::Matrix result;
    Transformation chain;
    chain.PushMatrix(m_dispatched_projtransform);
    chain.PushMatrix(final_view);
    chain.PushMatrix(m_worldtransform);
    chain.BuildResult();
    chain.GetResult(&result);

    Vector local_point = p_local_point;
    Vector t_local_point;
    result.transform(&local_point, &t_local_point);

    p_posx = (t_local_point[0] / (t_local_point[2] + 1.0));
    p_posy = (t_local_point[1] / (t_local_point[2] + 1.0));

    return (t_local_point[2] < 0.0 ? true : false);
}


dsreal TransformAspect::LocalPointDistanceFromCamera(const DrawSpace::Maths::Vector& p_local_point)
{
    DrawSpace::Maths::Matrix final_view;
    DrawSpace::Maths::Matrix inv;
    inv.identity();
    inv(2, 2) = -1.0;
    final_view = m_dispatched_viewtransform * inv;

    DrawSpace::Maths::Matrix result;
    Transformation chain;
    chain.PushMatrix(final_view);
    chain.PushMatrix(m_worldtransform);
    chain.BuildResult();
    chain.GetResult(&result);

    Vector local_point = p_local_point;
    Vector t_local_point;
    result.transform(&local_point, &t_local_point);

    return t_local_point.length();
}
