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

#include "sysengine.h"

using namespace renderMe::core;

void SystemEngine::run()
{
	for (auto& system : m_systems)
	{
		system.second.get()->run();
	}
}

System* SystemEngine::getSystem(int p_executionslot) const
{
	if (m_systems.count(p_executionslot))
	{
		return m_systems.at(p_executionslot).get();
	}
	else
	{
		_EXCEPTION("unknow system slot");
	}
}