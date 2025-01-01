
/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#include <string>
#include <vector>
#include "matrix.h"

namespace renderMe
{
	// resource for TriangleMeshes : Nodes and Bones hierarchy for supporting Meshes Animation

	struct SceneNode
	{
		std::string				    id;			// -> can include AnimationBone name (see above)
		std::string				    parent_id;
		std::vector<std::string>	children;
		core::maths::Matrix		    locale_transform;

		SceneNode()
		{
			locale_transform.identity();
		}
	};
}
