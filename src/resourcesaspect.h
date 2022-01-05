/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#pragma once

#include "componentcontainer.h"

namespace DrawSpace
{
namespace Aspect
{
class ResourcesAspect : public Core::ComponentContainer
{
public:

	struct AnimationDescription
	{
		dsstring	name;
		dsreal		ticks_per_seconds;
		dsreal		duration_seconds;
		int			num_channels;
	};

	struct MesheDescription
	{
		dsstring	node_id;
		dsstring	name;
		bool		has_positions;
		bool		has_faces;
		int         num_vertices;
		int         num_faces;
		bool		has_normales;
		bool		has_tbn;
		int  		num_bones;
		int			num_uvchannels;
	};

	struct MeshesFileDescription
	{
		dsstring							file;
		bool								has_meshes;
		int									num_meshes;

		bool								has_animations;
		int									num_animations;

		std::vector<AnimationDescription>	anims_descriptions;
		std::vector<MesheDescription>		meshes_descriptions;
	};

	void AddMeshesFileDescription(const MeshesFileDescription& p_descr);

	size_t GetMeshesFileDescriptionSize(void) const;
	MeshesFileDescription GetMeshesFileDescription(int p_index) const;

private:

	std::vector<MeshesFileDescription>	m_meshes_file_description;
	std::set<dsstring>					m_filenames_table;
};
}
}

