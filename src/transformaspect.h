/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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
class TransformAspect : public Core::Aspect
{
protected:
    DrawSpace::Utils::Matrix                                                        m_worldtransform;

    // transfos view et proj "distribuees" par le worldsystem
    DrawSpace::Utils::Matrix                                                        m_dispatched_viewtransform;
    DrawSpace::Utils::Matrix                                                        m_dispatched_projtransform;

    DrawSpace::Interface::AspectImplementations::TransformAspectImpl*               m_impl;
    TimeAspect*                                                                     m_time_aspect;


public:
    TransformAspect( void );

    void SetImplementation( DrawSpace::Interface::AspectImplementations::TransformAspectImpl* p_impl );
    void RemoveImplementation( void );
    
    void GetWorldTransform( DrawSpace::Utils::Matrix& p_worldtransform ) const;
    void GetViewTransform( DrawSpace::Utils::Matrix& p_viewtransform ) const;
    void GetProjTransform( DrawSpace::Utils::Matrix& p_projtransform ) const;
	TimeAspect* GetTimeAspectRef(void) const;

    void ComputeTransforms( Core::Entity* p_parent, Core::Entity* p_entity );
    void DispatchViewProj( const DrawSpace::Utils::Matrix& p_view, DrawSpace::Utils::Matrix& p_proj );

    void SetTimeAspect( TimeAspect* p_time_aspect );
};
}
}

#endif
