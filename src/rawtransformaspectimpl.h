/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#ifndef _RAWTRANSFORMASPECTIMPL_H_
#define _RAWTRANSFORMASPECTIMPL_H_

#include "transformaspectimpl.h"

namespace DrawSpace
{
namespace AspectImplementations
{
class RawTransformAspectImpl : public DrawSpace::Interface::AspectImplementations::TransformAspectImpl
{
public:
    virtual void GetLocaleTransform( Aspect::TransformAspect* p_transformaspect, Utils::Matrix& p_out_base_transform );
};
}
}
#endif
