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

#include "rawtransformaspectimpl.h"
#include "component.h"
#include "transformaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Maths;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;

void RawTransformAspectImpl::GetLocaleTransform( TransformAspect* p_transformaspect, Maths::Matrix& p_out_base_transform )
{
    ComponentList<Matrix> mats;
    p_transformaspect->GetComponentsByType<Matrix>( mats );

    Matrix cumul;
    cumul.identity();
    for( size_t i = 0; i < mats.size(); i++ )
    {
        Matrix curr_mat = mats[i]->getPurpose();    
        cumul = cumul * curr_mat;
    }

    p_out_base_transform = cumul;
}
