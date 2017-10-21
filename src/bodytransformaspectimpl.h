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

#ifndef _BODYTRANSFORMASPECTIMPL_H_
#define _BODYTRANSFORMASPECTIMPL_H_

#include "transformaspectimpl.h"

#include <btBulletDynamicsCommon.h>

namespace DrawSpace
{
namespace AspectImplementations
{
class BodyTransformAspectImpl : public DrawSpace::Interface::AspectImplementations::TransformAspectImpl
{
protected:
    btDefaultMotionState**                      m_motionState;
    Utils::Matrix*                              m_collider_mat;
    bool                                        m_collider_mode;
public:
    BodyTransformAspectImpl( btDefaultMotionState** p_motionState, Utils::Matrix* p_collider_mat );

    void SetColliderMode( bool p_collider_mode );
    virtual void GetLocaleTransform( Aspect::TransformAspect* p_transformaspect, Utils::Matrix& p_out_base_transform );
};
}
}
#endif