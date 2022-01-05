/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#ifndef _TRANSFORMASPECTIMPL_H_
#define _TRANSFORMASPECTIMPL_H_

#include "matrix.h"
#include "timemanager.h"
#include "timeaspect.h"

namespace DrawSpace
{
namespace Aspect
{
class TransformAspect;
}

namespace Interface
{
namespace AspectImplementations
{

class TransformAspectImpl abstract
{
protected:
    Aspect::TimeAspect* m_time_aspect;

public:
    TransformAspectImpl( void ) : m_time_aspect( NULL ) {};

    virtual void SetTimeAspect( Aspect::TimeAspect* p_time_aspect ) { m_time_aspect = p_time_aspect; };

    virtual void OnAddedInGraph(DrawSpace::Aspect::TransformAspect* p_transformaspect, const Utils::Matrix& p_parent_transform) {};
    virtual void OnRemovedFromGraph(DrawSpace::Aspect::TransformAspect* p_transformaspect, const Utils::Matrix& p_parent_transform) {};

    virtual void GetLocaleTransform( DrawSpace::Aspect::TransformAspect* p_transformaspect, Utils::Matrix& p_out_base_transform ) = 0;
};
}
}
}

#endif
