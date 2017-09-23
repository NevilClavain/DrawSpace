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

#ifndef _CAMERAASPECT_H_
#define _CAMERAASPECT_H_

#include "aspect.h"
#include "matrix.h"
#include "plugin.h"
#include "renderer.h"

namespace DrawSpace
{
namespace Aspect
{
class CameraAspect : public Core::Aspect
{
protected:
    DrawSpace::Utils::Matrix            m_viewtransform;
    DrawSpace::Utils::Matrix            m_projtransform;
    DrawSpace::Interface::Renderer*     m_renderer;

public:
    CameraAspect( void );
    void GetViewTransform( DrawSpace::Utils::Matrix& p_viewtransform );
    void GetProjTransform( DrawSpace::Utils::Matrix& p_projtransform );
};
}
}

#endif