
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

#include "entitygraph.h"
#include "entity.h"

#include "logsink.h"
#include "logconf.h"
#include "logging.h"


extern renderMe::core::logger::Sink localLogger("Helpers", renderMe::core::logger::Configuration::getInstance());

namespace renderMe
{
	namespace helpers
	{
		void logEntitygraph(core::Entitygraph& p_eg)
		{
			_RENDERME_DEBUG(localLogger, "Poueeeet");

			
			for (auto it = p_eg.preBegin(); it != p_eg.preEnd(); ++it)
			{
				const renderMe::core::Entity* current_entity { it->data() };
				const std::string currId{ current_entity->getId() };
				_RENDERME_DEBUG(localLogger, "Entity id : " + currId);
			}
			
		}
	}
}

