
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

#include "sysengine.h"
#include "resourcesystem.h"
#include "d3d11system.h"

using namespace mage;
using namespace mage::core;

void ModuleImpl::close(void)
{
	auto resourceSystem{ SystemEngine::getInstance()->getSystem(resourceSystemSlot) };
	auto resourceSystemInstance{ dynamic_cast<mage::ResourceSystem*>(resourceSystem) };

	resourceSystemInstance->killRunner();

	auto d3d11System{ SystemEngine::getInstance()->getSystem(d3d11SystemSlot) };
	auto d3d11SystemInstance{ dynamic_cast<mage::D3D11System*>(d3d11System) };

	d3d11SystemInstance->killRunner();
}