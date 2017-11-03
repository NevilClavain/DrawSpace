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

#include "bodytransformaspectimpl.h"
#include "component.h"
#include "transformaspect.h"
#include "bodyaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;

BodyTransformAspectImpl::BodyTransformAspectImpl( BodyAspect* p_aspect ) :
m_aspect( p_aspect )
{
}


void BodyTransformAspectImpl::GetLocaleTransform( TransformAspect* p_transformaspect, Utils::Matrix& p_out_base_transform )
{
    m_aspect->GetLastTransform( p_out_base_transform );
}