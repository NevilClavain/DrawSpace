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

#include "cameraaspect.h"
#include "renderer.h"

using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;

CameraAspect::CameraAspect( void )
{
    m_viewtransform.Identity();
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    // initialiser une matrice proj par defaut
    DrawSpace::Interface::Renderer::Characteristics characteristics;
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->GetRenderCharacteristics( characteristics );
    m_projtransform.Perspective( characteristics.width_viewport, characteristics.height_viewport, 1.0, 100000000000.0 ); 
}

void CameraAspect::GetViewTransform( DrawSpace::Utils::Matrix& p_viewtransform )
{
    p_viewtransform = m_viewtransform;
}

void CameraAspect::GetProjTransform( DrawSpace::Utils::Matrix& p_projtransform )
{
    p_projtransform = m_viewtransform;
}