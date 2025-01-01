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
#include <unordered_map>

#include "tvector.h"
#include "quaternion.h"

namespace renderMe
{
	// resources for Meshes Animations controls

	struct VectorKey
	{
		double							time_tick;
		core::maths::Real4Vector		value;
	};

	struct QuaternionKey
	{
		double							time_tick;
		core::maths::Quaternion			value;
	};

	struct NodeAnimation
	{
		std::string						node_name;
		std::vector<VectorKey>			position_keys;
		std::vector<VectorKey>			scaling_keys;
		std::vector<QuaternionKey>		rotations_keys;
	};

	using AnimationChannels = std::unordered_map<std::string, NodeAnimation>;

	struct AnimationKeys
	{
		std::string			name;
		double				ticks_per_seconds{ 0 };
		double				duration_seconds{ 0 };
		AnimationChannels	channels;
	};
}