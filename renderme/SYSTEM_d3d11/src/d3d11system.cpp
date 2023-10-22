
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

#pragma warning( disable : 4005 4838 )

#include "d3d11system.h"

#include "logsink.h"
#include "logconf.h"
#include "logging.h"

#include "aspects.h"
#include "entity.h"
#include "entitygraph.h"

#include "init.h"

#include "exceptions.h"


using namespace renderMe;
using namespace renderMe::core;


D3D11System::D3D11System(Entitygraph& p_entitygraph) : System(p_entitygraph)
{
}

void D3D11System::run()
{
	for (auto it = m_entitygraph.preBegin(); it != m_entitygraph.preEnd(); ++it)
	{
		const auto current_entity{ it->data() };
		const auto currId{ current_entity->getId() };

		if (current_entity->hasAspect(core::renderingAspect::id))
		{
			const auto& rendering_aspect{ current_entity->aspectAccess(core::renderingAspect::id) };

			auto rendering_target_comp{ rendering_aspect.getComponent<core::renderingAspect::renderingTarget>("renderingTarget") };
			if (rendering_target_comp)
			{
				auto& rendering_target{ rendering_target_comp->getPurpose() };

				if (core::renderingAspect::renderingTarget::WINDOW_TARGET == rendering_target)
				{					
					if (!m_initialized)
					{						
						if (renderMe::d3d11::helpers::init(current_entity))
						{
							m_initialized = true;
						}
						else
						{
							_EXCEPTION("D3D11 initialization failed")
						}
					}
				}
			}
		}
	}
}





