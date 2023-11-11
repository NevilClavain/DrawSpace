
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

#include <functional>
#include <vector>

#include "aspects.h"
#include "entity.h"
#include "entitygraph.h"

namespace renderMe
{
	namespace helpers
	{		
		template<typename AspectT>
		void extractAspectsTopDown(core::Entitygraph& p_eg, const std::function<void(core::Entity* p_entity, const core::ComponentContainer& p_aspect)>& p_func)
		{
			for (auto it = p_eg.preBegin(); it != p_eg.preEnd(); ++it)
			{
				const auto current_entity{ it->data() };
				const auto currId{ current_entity->getId() };

				if (current_entity->hasAspect(AspectT::id))
				{
					const auto& aspect{ current_entity->aspectAccess(AspectT::id) };
					p_func(current_entity, aspect);
				}
			}
		}

		template<typename AspectT>
		void extractAspectsDownTop(core::Entitygraph& p_eg, const std::function<void(core::Entity* p_entity, const core::ComponentContainer& p_aspect)>& p_func)
		{
			for (auto it = p_eg.postBegin(); it != p_eg.postEnd(); ++it)
			{
				const auto current_entity{ it->data() };
				const auto currId{ current_entity->getId() };

				if (current_entity->hasAspect(AspectT::id))
				{
					const auto& aspect{ current_entity->aspectAccess(AspectT::id) };
					p_func(current_entity, aspect);
				}
			}
		}
	}
}
