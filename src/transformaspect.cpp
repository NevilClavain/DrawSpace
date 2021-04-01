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
    m_dispatched_viewtransform.Identity();
    m_dispatched_projtransform.Identity();
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
    Matrix finaltransform_mat;

    locale_mat.Identity();

    bool ignore_parent_transform = false;

    // operation : pour une liste de N matrices
    // [N] * [N-1] * [N - 2] * ..... * [2] * [1] * [0]

    // la 1ERE matrice appliquee est la matrice N

    for (auto& e : m_impls_list)
    {
        Matrix res, current;

        e.second->GetLocaleTransform(this, current);

        Matrix::MatrixMult(&current, &locale_mat, &res);
        locale_mat = res;

        if (e.second->IgnoreParentTransformation())
        {
            ignore_parent_transform = true;
        }
    }

    if( p_parent )
    {
        Matrix parent_finaltransform_mat;
        TransformAspect* parent_world_aspect = p_parent->GetAspect<TransformAspect>();

        if( parent_world_aspect && !ignore_parent_transform )
        {
            finaltransform_mat = locale_mat * parent_world_aspect->m_worldtransform;
        }
        else
        {
            finaltransform_mat = locale_mat;
        }
    }
    else
    {
        finaltransform_mat = locale_mat;
    }

    m_worldtransform = finaltransform_mat;
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

}

void TransformAspect::OnRemovedFromGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph, Entity* p_parent_entity)
{

}
