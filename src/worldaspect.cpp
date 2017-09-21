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

#include "worldaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

WorldAspect::WorldAspect( void )
{
    m_worldtransform.Identity();
}

void WorldAspect::AddImplementation( WorldAspectImpl* p_impl )
{
    m_impls.push_back( p_impl );
}

void WorldAspect::ComputeTransforms( Entity* p_parent, Entity* p_entity )
{
    DrawSpace::Utils::Matrix locale_mat;
    DrawSpace::Utils::Matrix finaltransform_mat;

    locale_mat.Identity();

    Matrix cumul;
    cumul.Identity();
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        Matrix curr;
        m_impls[i]->get_locale_transform( this, curr );
        cumul = cumul * curr;
    }

    locale_mat = locale_mat * cumul;

    if( p_parent )
    {
        DrawSpace::Utils::Matrix parent_finaltransform_mat;
        WorldAspect* parent_world_aspect = p_parent->GetAspect<WorldAspect>();

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

void WorldAspect::GetWorldTransform( DrawSpace::Utils::Matrix& p_worldtransform )
{
    p_worldtransform = m_worldtransform;
}
