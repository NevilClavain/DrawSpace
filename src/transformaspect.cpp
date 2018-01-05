/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

TransformAspect::TransformAspect( void ) :
m_impl( NULL ),
m_time_aspect( NULL )
{
    m_worldtransform.Identity();
    m_dispatched_viewtransform.Identity();
    m_dispatched_projtransform.Identity();
}

void TransformAspect::SetImplementation( AspectImplementations::TransformAspectImpl* p_impl )
{
    m_impl = p_impl;
    if( m_time_aspect )
    {
        m_impl->SetTimeAspect( m_time_aspect );
    }
}


void TransformAspect::ComputeTransforms( Entity* p_parent, Entity* p_entity )
{
    Matrix locale_mat;
    Matrix finaltransform_mat;

    locale_mat.Identity();

    bool ignore_parent_transform = false;

    if( m_impl )
    {
        m_impl->GetLocaleTransform( this, locale_mat );
        ignore_parent_transform = m_impl->IgnoreParentTransformation();
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

void TransformAspect::SetTimeAspect( TimeAspect* p_time_aspect )
{
    m_time_aspect = p_time_aspect;
    if( m_impl )
    {
        m_impl->SetTimeAspect( p_time_aspect );
    }
}
