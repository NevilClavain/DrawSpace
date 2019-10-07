#pragma once

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

#include "aspect.h"
#include "matrix.h"
#include "quaternion.h"

namespace DrawSpace
{
namespace Aspect
{
class AnimationsAspect : public Core::Aspect 
{
public:

struct Node
{
	dsstring				id;
	dsstring				parent_id;
	std::vector<dsstring>	children;
	Utils::Matrix			locale_transform;

	Node(void)
	{
		locale_transform.Identity();
	}
};

struct BoneOutput
{
	Utils::Matrix offset_matrix;
	Utils::Matrix final_transformation;

	BoneOutput(void)
	{
		offset_matrix.Identity();
		final_transformation.Identity();
	}
};

struct VectorKey
{

	dsreal						time_tick;
	Utils::Vector				value;
};

struct QuaternionKey
{
	dsreal						time_tick;
	Utils::Quaternion			value;
};

struct NodeAnimation
{
	dsstring					node_name;
	std::vector<VectorKey>		position_keys;
	std::vector<VectorKey>		scaling_keys;
	std::vector<QuaternionKey>	rotations_keys;
};

struct AnimationRoot
{
	dsreal						ticksPerSeconds;
	dsreal						duration;
	std::vector<NodeAnimation>	channels;
};

};
}
}
