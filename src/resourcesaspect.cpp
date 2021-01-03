/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#include "resourcesaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Aspect;

size_t ResourcesAspect::GetMeshesFileDescriptionSize(void) const
{
	return m_meshes_file_description.size();
}

ResourcesAspect::MeshesFileDescription ResourcesAspect::GetMeshesFileDescription(int p_index) const
{
	return m_meshes_file_description[p_index];
}

void ResourcesAspect::AddMeshesFileDescription(const MeshesFileDescription& p_descr)
{
	if (0 == m_filenames_table.count(p_descr.file))
	{
		m_meshes_file_description.push_back(p_descr);
		m_filenames_table.insert(p_descr.file);
	}
}
