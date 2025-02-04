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

#include "aspects.h"
#include "sysengine.h"
#include "d3d11system.h"
#include "timesystem.h"
#include "syncvariable.h"
#include "timemanager.h"
#include "resourcesystem.h"
#include "renderingqueuesystem.h"
#include "worldsystem.h"
#include "worldposition.h"
#include "shader.h"
#include "linemeshe.h"
#include "trianglemeshe.h"
#include "renderstate.h"

#include "dataprintsystem.h"

#include "logger_service.h"

#include "filesystem.h"
#include "logconf.h"

#include "entity.h"

#include "datacloud.h"

#include "animatorfunc.h"
#include "animators_helpers.h"


using namespace mage;
using namespace mage::core;
using namespace mage::rendering;


ModuleImpl::ModuleImpl()
{
	/////////// create common specific logger for events
	services::LoggerSharing::getInstance()->createLogger("Events");

}

std::string ModuleImpl::getModuleName() const
{
	return "Animations";
}

std::string ModuleImpl::getModuleDescr() const
{
	return "Animations module example";
}

mage::core::Entitygraph* ModuleImpl::entitygraph()
{
	return &m_entitygraph;
}

void ModuleImpl::onAppEvent(WPARAM p_wParam, LPARAM p_lParam)
{
}

void ModuleImpl::registerSubscriber(const Callback& p_callback)
{
	mage::property::EventSource<mage::interfaces::ModuleEvents, int>::registerSubscriber(p_callback);

	// send immediately m_show_mouse_cursor value
	for (const auto& call : m_callbacks)
	{
		call(mage::interfaces::ModuleEvents::MOUSE_DISPLAY_CHANGED, (int)m_show_mouse_cursor);
	}

	// send immediately m_mouse_circular_mode value
	for (const auto& call : m_callbacks)
	{
		call(mage::interfaces::ModuleEvents::MOUSE_CIRCULARMODE_CHANGED, (int)m_mouse_circular_mode);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

