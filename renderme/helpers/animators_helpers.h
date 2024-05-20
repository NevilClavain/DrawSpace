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
#include "tvector.h"
#include "matrix.h"
#include "quaternion.h"
#include "syncvariable.h"
#include "timemanager.h"

namespace renderMe
{
	namespace helpers
	{
		namespace animators
		{
			auto makeYRotationJointAnimator()
			{
				const auto animator
				{
					[](const core::ComponentContainer& p_world_aspect,
						const core::ComponentContainer& p_time_aspect,
						const transform::WorldPosition& p_parent_pos,
						const std::unordered_map<std::string, std::string>& p_keys)
					{
						const auto& y_rotation_angle{ p_time_aspect.getComponent<core::SyncVariable>(p_keys.at("yRotJointAnim.angle"))->getPurpose()};

						core::maths::Matrix rotation_mat;
						rotation_mat.rotation(core::maths::Real3Vector(0.0, 1.0, 0.0), y_rotation_angle.value);

						transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>(p_keys.at("yRotJointAnim.output"))->getPurpose() };
						wp.local_pos = wp.local_pos * rotation_mat;
					}
				};

				return animator;
			}

			auto makeXYZSliderJointAnimator()
			{
				const auto animator
				{
					[](const core::ComponentContainer& p_world_aspect,
						const core::ComponentContainer& p_time_aspect,
						const transform::WorldPosition& p_parent_pos,
						const std::unordered_map<std::string, std::string>& p_keys)
					{
						const auto& x_pos{ p_time_aspect.getComponent<core::SyncVariable>(p_keys.at("xyzSliderJointAnim.x_pos"))->getPurpose()};
						const auto& y_pos{ p_time_aspect.getComponent<core::SyncVariable>(p_keys.at("xyzSliderJointAnim.y_pos"))->getPurpose()};
						const auto& z_pos{ p_time_aspect.getComponent<core::SyncVariable>(p_keys.at("xyzSliderJointAnim.z_pos"))->getPurpose() };

						core::maths::Matrix translation_mat;
						translation_mat.translation(x_pos.value, y_pos.value, z_pos.value);

						transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>(p_keys.at("xyzSliderJointAnim.output"))->getPurpose() };
						wp.local_pos = wp.local_pos * translation_mat;
					}
				};

				return animator;
			}


			auto makeGimbalLockJointAnimator()
			{
				const auto animator
				{
					[](const core::ComponentContainer& p_world_aspect,
						const core::ComponentContainer& p_time_aspect,
						const transform::WorldPosition& p_parent_pos,
						const std::unordered_map<std::string, std::string>& p_keys)
					{

						const double theta{ p_world_aspect.getComponent<double>(p_keys.at("gimbalLockJointAnim.theta"))->getPurpose() };
						const double phi{ p_world_aspect.getComponent<double>(p_keys.at("gimbalLockJointAnim.phi"))->getPurpose() }; // to be continued...
						

						auto& pos{ p_world_aspect.getComponent<core::maths::Real3Vector>(p_keys.at("gimbalLockJointAnim.position"))->getPurpose() };
						core::maths::Matrix positionmat;
						positionmat.translation(pos);
				
						core::maths::Quaternion		    qyaw;
						core::maths::Quaternion		    qpitch;
						
						qyaw.rotationAxis(core::maths::YAxisVector, theta);
						qpitch.rotationAxis(core::maths::XAxisVector, phi);

						const auto qres{ qpitch * qyaw };

						core::maths::Matrix orientation;
						qres.rotationMatFrom(orientation);

						// store result
						transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>(p_keys.at("gimbalLockJointAnim.output"))->getPurpose() };
						wp.local_pos = wp.local_pos * orientation * positionmat;

						// update pos with speed
						const double fps_speed{ p_world_aspect.getComponent<double>(p_keys.at("gimbalLockJointAnim.speed"))->getPurpose() };
						if (std::abs(fps_speed) > 0.0)
						{
							//project speed vector in global coords

							// on neg z axis...
							core::maths::Real4Vector local_speed(0.0, 0.0, -fps_speed, 1.0);
							core::maths::Real4Vector global_speed;

							orientation.transform(&local_speed, &global_speed);

							core::maths::Real3Vector global_speed3(global_speed[0], global_speed[1], global_speed[2]);
						
							pos = pos + global_speed3;
						}
					}
				};

				return animator;								
			}

			auto makeFullGimbalJointAnimator()
			{
				const auto animator
				{
					[](const core::ComponentContainer& p_world_aspect,
						const core::ComponentContainer& p_time_aspect,
						const transform::WorldPosition& p_parent_pos,
						const std::unordered_map<std::string, std::string>& p_keys)
					{
						
						auto& pos{ p_world_aspect.getComponent<core::maths::Real3Vector>(p_keys.at("fullGimbalJointAnim.position"))->getPurpose() };
						core::maths::Matrix positionmat;
						positionmat.translation(pos);


						//vitesses demandees...

						const double rspeed_x{ p_world_aspect.getComponent<double>(p_keys.at("fullGimbalJointAnim.rot_speed_x"))->getPurpose() };
						const double rspeed_y{ p_world_aspect.getComponent<double>(p_keys.at("fullGimbalJointAnim.rot_speed_y"))->getPurpose() };
						const double rspeed_z{ p_world_aspect.getComponent<double>(p_keys.at("fullGimbalJointAnim.rot_speed_z"))->getPurpose() };

						// axe demandes
						auto& rot_axis_x{ p_world_aspect.getComponent<core::maths::Real3Vector>(p_keys.at("fullGimbalJointAnim.rot_axis_x"))->getPurpose() };
						auto& rot_axis_y{ p_world_aspect.getComponent<core::maths::Real3Vector>(p_keys.at("fullGimbalJointAnim.rot_axis_y"))->getPurpose() };
						auto& rot_axis_z{ p_world_aspect.getComponent<core::maths::Real3Vector>(p_keys.at("fullGimbalJointAnim.rot_axis_z"))->getPurpose() };

						// current quat

						auto& current_res{ p_world_aspect.getComponent<core::maths::Quaternion>(p_keys.at("fullGimbalJointAnim.quat"))->getPurpose() };

						// work variables
						core::maths::Matrix orientation;
						const auto tm{ core::TimeManager::getInstance() };
						double fps;
						core::maths::Quaternion q_axis;


						/// NB: l'ordre dans lequel sont traites les axes n'a pas d'importance...

						/////////////////Axe X /////////////////////////////

						fps = tm->convertUnitPerSecFramePerSec(rspeed_x);
						q_axis.rotationAxis(rot_axis_x, fps);
						current_res = current_res * q_axis;

						current_res.rotationMatFrom(orientation);
						rot_axis_x[0] = orientation(0, 0);
						rot_axis_x[1] = orientation(0, 1);
						rot_axis_x[2] = orientation(0, 2);

						/////////////////Axe Y /////////////////////////////

						fps = tm->convertUnitPerSecFramePerSec(rspeed_y);
						q_axis.rotationAxis(rot_axis_y, fps);
						current_res = current_res * q_axis;

						current_res.rotationMatFrom(orientation);
						rot_axis_y[0] = orientation(1, 0);
						rot_axis_y[1] = orientation(1, 1);
						rot_axis_y[2] = orientation(1, 2);

						/////////////////Axe Z /////////////////////////////

						fps = tm->convertUnitPerSecFramePerSec(rspeed_z);
						q_axis.rotationAxis(rot_axis_z, fps);
						current_res = current_res * q_axis;

						current_res.rotationMatFrom(orientation);
						rot_axis_z[0] = orientation(2, 0);
						rot_axis_z[1] = orientation(2, 1);
						rot_axis_z[2] = orientation(2, 2);

						// store result
						transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>(p_keys.at("fullGimbalJointAnim.output"))->getPurpose() };
						wp.local_pos = wp.local_pos * orientation * positionmat;

						const double speed{ p_world_aspect.getComponent<double>(p_keys.at("fullGimbalJointAnim.speed"))->getPurpose() };
						if (std::abs(speed) > 0.0)
						{
							//project speed vector in global coords

							// on neg z axis...
							core::maths::Real4Vector local_speed(0.0, 0.0, -speed, 1.0);
							core::maths::Real4Vector global_speed;

							orientation.transform(&local_speed, &global_speed);

							core::maths::Real3Vector global_speed3(global_speed[0], global_speed[1], global_speed[2]);

							pos = pos + global_speed3;
						}

					}
				};

				return animator;
			}

			auto makeLookatJointAnimator()
			{
				const auto animator
				{
					[](const core::ComponentContainer& p_world_aspect,
						const core::ComponentContainer& p_time_aspect,
						const transform::WorldPosition& p_parent_pos,
						const std::unordered_map<std::string, std::string>& p_keys)
					{
						transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>(p_keys.at("lookatJointAnim.output"))->getPurpose() };

						// compute source (current node) absolute position : parent global pos + actual loca pos
						core::maths::Real4Vector pos(0.0, 0.0, 0.0, 1.0);												
						core::maths::Real4Vector source4;
						const core::maths::Matrix absolute_transformation{ wp.local_pos * p_parent_pos.global_pos };
												
						absolute_transformation.transform(&pos, &source4);

						core::maths::Real3Vector source3(source4[0], source4[1], source4[2]);

						const auto dest { p_world_aspect.getComponent<core::maths::Real3Vector>(p_keys.at("lookatJointAnim.dest"))->getPurpose()};

						// vector from dest to source
						const core::maths::Real3Vector forward(source3[0] - dest[0], source3[1] - dest[1], source3[2] - dest[2]);						
						const auto quat{ core::maths::Quaternion::lookRotation(forward, core::maths::YAxisVector) };

						core::maths::Matrix orientation;
						quat.rotationMatFrom(orientation);

						core::maths::Matrix translation_from_parent; 
						translation_from_parent.translation( p_parent_pos.global_pos.getPosition() );

						core::maths::Matrix translation_from_current_local;
						translation_from_current_local.translation(wp.local_pos.getPosition());

					
						// store result
						
						wp.composition_operation = transform::WorldPosition::TransformationComposition::TRANSFORMATION_ABSOLUTE;

						wp.local_pos = orientation * translation_from_current_local * translation_from_parent;
					}
				};

				return animator;
			}

			auto makeOscillationJointAnimator()
			{
				const auto animator
				{
					[](const core::ComponentContainer& p_world_aspect,
						const core::ComponentContainer& p_time_aspect,
						const transform::WorldPosition& p_parent_pos,
						const std::unordered_map<std::string, std::string>& p_keys)
					{
						transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>(p_keys.at("oscillationJointAnim.output"))->getPurpose() };

						const auto& osc1{ p_time_aspect.getComponent<core::SyncVariable>(p_keys.at("oscillationJointAnim.oscillation1"))->getPurpose() };
						const auto& osc2{ p_time_aspect.getComponent<core::SyncVariable>(p_keys.at("oscillationJointAnim.oscillation2"))->getPurpose() };

						const core::maths::Quaternion quat1{ p_world_aspect.getComponent<core::maths::Quaternion>(p_keys.at("oscillationJointAnim.quat1"))->getPurpose() };
						const core::maths::Quaternion quat2{ p_world_aspect.getComponent<core::maths::Quaternion>(p_keys.at("oscillationJointAnim.quat2"))->getPurpose() };
						const core::maths::Quaternion quat3{ p_world_aspect.getComponent<core::maths::Quaternion>(p_keys.at("oscillationJointAnim.quat3"))->getPurpose() };

						const auto final_quat{ core::maths::Quaternion::lerp( std::move(core::maths::Quaternion::lerp(quat1, quat2, osc1.value)), quat3, osc2.value) };
						
						core::maths::Matrix orientation;
						final_quat.rotationMatFrom(orientation);

		
						wp.local_pos = wp.local_pos * orientation;
					}
				};

				return animator;
			}
		}
	}
}