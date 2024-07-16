
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



using namespace renderMe;
using namespace renderMe::core;
using namespace renderMe::rendering;

void ModuleImpl::run(void)
{

	const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };

	/////////////////////////////////////////////////////

	auto sysEngine{ SystemEngine::getInstance() };
	sysEngine->run();

	/////////////////////////////////////////////////////

	//const auto currentFPS { m_timeInfos_time_aspect->getComponent<int>("framePerSeconds")->getPurpose() };
	const auto currentFPS{ dataCloud->readDataValue<long>("std.framesPerSecond") };

	const std::string fpsText{ std::string("fps = ") + std::to_string(currentFPS) };

	//m_windowRenderingQueue->setText(4, { fpsText, "CourierNew.10.spritefont", { 255, 0, 0, 255 }, { 0, 0 }, 0.0f });

	// resources system event
	m_windowRenderingQueue->setText(5, { m_resources_event, "CourierNew.10.spritefont", {255, 255, 255, 255}, {0, 120}, 0.0 });

}