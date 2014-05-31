/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "camera.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

Camera::Camera( const dsstring& p_name ) : TransformNode( p_name )
{


}


Camera::~Camera( void )
{

}


void Camera::OnRegister( Scenegraph* p_scenegraph )
{
    std::map<dsstring, Core::TransformNode*>& camera_list = p_scenegraph->GetCamerasList();
    camera_list[m_scenename] = this;
}