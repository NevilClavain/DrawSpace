
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

#include "module_impl.h"

#include "sysengine.h"
#include "resourcesystem.h"
#include "d3d11system.h"

using namespace renderMe;
using namespace renderMe::core;

void ModuleImpl::close(void)
{
	auto resourceSystem{ SystemEngine::getInstance()->getSystem(2) };
	auto resourceSystemInstance{ dynamic_cast<renderMe::ResourceSystem*>(resourceSystem) };

	resourceSystemInstance->killRunner();

	auto d3d11System{ SystemEngine::getInstance()->getSystem(1) };
	auto d3d11SystemInstance{ dynamic_cast<renderMe::D3D11System*>(d3d11System) };

	d3d11SystemInstance->killRunner();
}