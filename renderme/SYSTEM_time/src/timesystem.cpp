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

#include "timesystem.h"
#include "entity.h"
#include "entitygraph.h"
#include "aspects.h"
#include "ecshelpers.h"

using namespace renderMe;
using namespace renderMe::core;

TimeSystem::TimeSystem(Entitygraph& p_entitygraph) : System(p_entitygraph)
{
}

void TimeSystem::run()
{
	m_tm.update();

	if (m_tm.isReady())
	{
		const auto forEachTimeAspect
		{
			[&](Entity* p_entity, const ComponentContainer& p_time_aspect)
			{
				// update FPS info
				const auto fpsComp { p_time_aspect.getComponent<int>("framePerSeconds") };
				if (fpsComp)
				{
					fpsComp->getPurpose() = m_tm.getFPS();
				}				
			}
		};

		renderMe::helpers::extractAspectsTopDown<renderMe::core::timeAspect>(m_entitygraph, forEachTimeAspect);
	}
}