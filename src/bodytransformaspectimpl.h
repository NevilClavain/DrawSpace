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


#ifndef _BODYTRANSFORMASPECTIMPL_H_
#define _BODYTRANSFORMASPECTIMPL_H_

#include "transformaspectimpl.h"

#include <btBulletDynamicsCommon.h>

namespace DrawSpace
{
namespace Aspect
{
class BodyAspect;
}
namespace AspectImplementations
{
class BodyTransformAspectImpl : public DrawSpace::Interface::AspectImplementations::TransformAspectImpl
{
protected:

    Aspect::BodyAspect*   m_aspect;

public:
    BodyTransformAspectImpl( Aspect::BodyAspect* p_aspect );

    virtual void GetLocaleTransform( Aspect::TransformAspect* p_transformaspect, Utils::Matrix& p_out_base_transform );
    //virtual bool IgnoreParentTransformation( void ) const { return true; };
    virtual bool IgnoreParentTransformation( void ) const;
};
}
}
#endif
