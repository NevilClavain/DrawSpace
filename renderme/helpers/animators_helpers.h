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
#include <unordered_map>
#include "componentcontainer.h"
#include "worldposition.h"
#include "matrix.h"
#include "syncvariable.h"

namespace renderMe
{
	namespace helpers
	{
		namespace animators
		{
			auto makeSynchronizedYRotationAnimator()
			{
				const auto animator
				{
					[](const core::ComponentContainer& p_world_aspect,
						const core::ComponentContainer& p_time_aspect,
						const transform::WorldPosition& p_parent_pos,
						const std::unordered_map<std::string, std::string>& p_keys)
					{
						const auto& y_rotation_angle{ p_time_aspect.getComponent<core::SyncVariable>(p_keys.at("syncYRot.angle"))->getPurpose()};

						core::maths::Matrix rotation_mat;
						rotation_mat.rotation(core::maths::Real4Vector(0.0, 1.0, 0.0), y_rotation_angle.value);

						transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("position")->getPurpose() };
						wp.local_pos = wp.local_pos * rotation_mat;
					}
				};

				return animator;
			}

			auto makeFPSAnimator() 
			{
				const auto animator
				{
					[](const core::ComponentContainer& p_world_aspect,
						const core::ComponentContainer& p_time_aspect,
						const transform::WorldPosition& p_parent_pos,
						const std::unordered_map<std::string, std::string>& p_keys)
					{

						const double fps_theta{ p_world_aspect.getComponent<double>(p_keys.at("fpsAnim.theta"))->getPurpose() };
						const double fps_phi{ p_world_aspect.getComponent<double>(p_keys.at("fpsAnim.phi"))->getPurpose() }; // to be continued...

						core::maths::Matrix fps_thetarotnmat;
						fps_thetarotnmat.rotation(core::maths::Real4Vector(0.0, 1.0, 0.0), fps_theta);

						core::maths::Matrix fps_phirotnmat;
						fps_phirotnmat.rotation(core::maths::Real4Vector(1.0, 0.0, 0.0), fps_phi);

						auto& fps_pos { p_world_aspect.getComponent<core::maths::Real4Vector>(p_keys.at("fpsAnim.position"))->getPurpose() };

						core::maths::Matrix fps_positionmat;
						fps_positionmat.translation(fps_pos);

						const auto final_local_mat{ fps_phirotnmat * fps_thetarotnmat * fps_positionmat };

						// store result
						transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>(p_keys.at("fpsAnim.output"))->getPurpose() };
						wp.local_pos = final_local_mat;

						// update pos with speed
						const double fps_speed{ p_world_aspect.getComponent<double>(p_keys.at("fpsAnim.speed"))->getPurpose() };
						if (std::abs(fps_speed) > 0.0)
						{
							//project speed vector in global coords

							// on neg z axis...
							core::maths::Real4Vector local_speed(0.0, 0.0, -fps_speed, 1.0);
							core::maths::Real4Vector global_speed;

							const auto final_mat{ fps_phirotnmat * fps_thetarotnmat * p_parent_pos.global_pos };
							final_mat.transform(&local_speed, &global_speed);

							fps_pos = fps_pos + global_speed;

						}
					}
				};

				return animator;
								
			}
		}
	}
}