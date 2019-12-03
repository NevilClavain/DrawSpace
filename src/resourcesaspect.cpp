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

#include "resourcesaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Aspect;

ResourcesAspect::MeshesFileDescription ResourcesAspect::GetMeshesFileDescription(const dsstring& p_filename) const
{
	if (m_meshes_file_description.count(p_filename))
	{
		return m_meshes_file_description.at(p_filename);
	}
	else
	{
		_DSEXCEPTION("unknown filename key entry in meshe file description file table")
	}
}

void ResourcesAspect::AddMeshesFileDescription(const MeshesFileDescription& p_descr)
{
	m_meshes_file_description[p_descr.file] = p_descr;
}