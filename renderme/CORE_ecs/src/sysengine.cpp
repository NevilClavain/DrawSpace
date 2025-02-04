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

#include "sysengine.h"

using namespace mage::core;

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