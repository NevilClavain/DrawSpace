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

#include "transformaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

TransformAspect::TransformAspect( void )
{
    m_worldtransform.Identity();
    m_dispatched_viewtransform.Identity();
    m_dispatched_projtransform.Identity();
}

void TransformAspect::AddImplementation( AspectImplementations::WorldAspectImpl* p_impl )
{
    m_impls.push_back( p_impl );
}

void TransformAspect::ComputeTransforms( Entity* p_parent, Entity* p_entity )
{
    Matrix locale_mat;
    Matrix finaltransform_mat;

    locale_mat.Identity();

    Matrix cumul;
    cumul.Identity();
    
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        Matrix curr;
        m_impls[i]->GetLocaleTransform( this, curr );
        cumul = cumul * curr;
    }
   
    locale_mat = locale_mat * cumul;

    if( p_parent )
    {
        Matrix parent_finaltransform_mat;
        TransformAspect* parent_world_aspect = p_parent->GetAspect<TransformAspect>();

        if( parent_world_aspect )
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

void TransformAspect::GetWorldTransform( Matrix& p_worldtransform )
{
    p_worldtransform = m_worldtransform;
}

void TransformAspect::GetViewTransform( DrawSpace::Utils::Matrix& p_viewtransform )
{
    p_viewtransform = m_dispatched_viewtransform;
}

void TransformAspect::GetProjTransform( DrawSpace::Utils::Matrix& p_projtransform )
{
    p_projtransform = m_dispatched_projtransform;
}
