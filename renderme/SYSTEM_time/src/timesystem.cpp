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

#include "timesystem.h"
#include "entity.h"
#include "entitygraph.h"
#include "aspects.h"
#include "ecshelpers.h"
#include "syncvariable.h"
#include "timecontrol.h"


using namespace mage;
using namespace mage::core;

TimeSystem::TimeSystem(Entitygraph& p_entitygraph) : System(p_entitygraph)
{
}

void TimeSystem::run()
{
	auto tc{ TimeControl::getInstance() };
	tc->update();
	if (tc->isReady())
	{

		const auto forEachTimeAspect
		{
			[&](Entity* p_entity, const ComponentContainer& p_time_components)
			{
				// search for TimeManager::Variable objects

				const auto syncvars_list{ p_time_components.getComponentsByType<SyncVariable>() };
				if (syncvars_list.size())
				{
					for (auto& v : syncvars_list)
					{
						tc->manageVariable(v->getPurpose());
					}
				}
			}
		};

		mage::helpers::extractAspectsTopDown<mage::core::timeAspect>(m_entitygraph, forEachTimeAspect);
	}
}