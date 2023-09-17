

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

#include "app.h"

#include "filesystem.h"
#include "logsink.h"
#include "logconf.h"
#include "logging.h"


using namespace renderMe;
using namespace renderMe::core;

static renderMe::core::logger::Sink localLogger("App", renderMe::core::logger::Configuration::getInstance());

App::App()
{

}

bool App::initApp(HINSTANCE p_hInstance)
{
	renderMe::core::FileContent<char> logConfFileContent("./rt_config/logrt.json");
	logConfFileContent.load();

	const auto dataSize{ logConfFileContent.getDataSize() };
	const std::string data(logConfFileContent.getData(), dataSize);

	renderMe::core::Json jsonParser;
	jsonParser.registerSubscriber(renderMe::core::logger::Configuration::getInstance()->getCallback());

	const auto parseStatus{ jsonParser.parse(data) };

	_RENDERME_DEBUG(localLogger, "InitApp startup");

	return true;
}

void App::idleApp(void)
{
}

bool App::initRenderer(void)
{
	return true;
}

void App::stopRenderer(void)
{

}
