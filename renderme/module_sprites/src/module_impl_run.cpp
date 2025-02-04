
/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include "module_impl.h"
#include <string>

#include "aspects.h"
#include "datacloud.h"
#include "sysengine.h"

#include "trianglemeshe.h"
#include "renderstate.h"
#include "texture.h"

#include "syncvariable.h"

#include "worldposition.h"
#include "animatorfunc.h"
#include "animators_helpers.h"


using namespace mage;
using namespace mage::core;
using namespace mage::rendering;


void ModuleImpl::manage_sprite_trajectory(core::Entity* p_entity)
{
	const auto& time_aspect{ p_entity->aspectAccess(timeAspect::id) };
	core::SyncVariable& x_pos{ time_aspect.getComponent<SyncVariable>("x_pos")->getPurpose() };
	core::SyncVariable& y_pos{ time_aspect.getComponent<SyncVariable>("y_pos")->getPurpose() };
	core::SyncVariable& z_rot{ time_aspect.getComponent<SyncVariable>("z_rot")->getPurpose() };
	

	// sprite initial launch
	if (x_pos.step == 0.0 && y_pos.step == 0.0)
	{
		x_pos.step = (*m_speed_distribution)(*m_generator);
		x_pos.direction = (*m_speed_sign_distribution)(*m_generator) == 0 ? SyncVariable::Direction::DEC : SyncVariable::Direction::INC;

		y_pos.step = (*m_speed_distribution)(*m_generator);
		y_pos.direction = (*m_speed_sign_distribution)(*m_generator) == 0 ? SyncVariable::Direction::DEC : SyncVariable::Direction::INC;

		z_rot.step = (*m_rotation_speed_distribution)(*m_generator);
		z_rot.direction = (*m_rotation_speed_sign_distribution)(*m_generator) == 0 ? SyncVariable::Direction::DEC : SyncVariable::Direction::INC;		
	}

	// sprite collisions management

	const auto dataCloud{ mage::rendering::Datacloud::getInstance() };
	const auto viewport{ dataCloud->readDataValue<maths::FloatCoords2D>("std.viewport") };

	if (x_pos.value > viewport.x() * 0.5)
	{
		x_pos.direction = SyncVariable::Direction::DEC;

		z_rot.step = (*m_rotation_speed_distribution)(*m_generator);
		z_rot.direction = (*m_rotation_speed_sign_distribution)(*m_generator) == 0 ? SyncVariable::Direction::DEC : SyncVariable::Direction::INC;
	}
	else if (x_pos.value < -viewport.x() * 0.5)
	{
		x_pos.direction = SyncVariable::Direction::INC;

		z_rot.step = (*m_rotation_speed_distribution)(*m_generator);
		z_rot.direction = (*m_rotation_speed_sign_distribution)(*m_generator) == 0 ? SyncVariable::Direction::DEC : SyncVariable::Direction::INC;
	}

	if (y_pos.value > viewport.y() * 0.5)
	{
		y_pos.direction = SyncVariable::Direction::DEC;

		z_rot.step = (*m_rotation_speed_distribution)(*m_generator);
		z_rot.direction = (*m_rotation_speed_sign_distribution)(*m_generator) == 0 ? SyncVariable::Direction::DEC : SyncVariable::Direction::INC;
	}
	else if (y_pos.value < -viewport.y() * 0.5)
	{
		y_pos.direction = SyncVariable::Direction::INC;

		z_rot.step = (*m_rotation_speed_distribution)(*m_generator);
		z_rot.direction = (*m_rotation_speed_sign_distribution)(*m_generator) == 0 ? SyncVariable::Direction::DEC : SyncVariable::Direction::INC;
	}

}

void ModuleImpl::run(void)
{
	/////////////////////////////////////////////////////

	auto sysEngine{ SystemEngine::getInstance() };
	sysEngine->run();

	/////////////////////////////////////////////////////

	for (auto sprite : m_sprites)
	{
		manage_sprite_trajectory(sprite);
	}
	/////////////////////////////////////////////////////	
}