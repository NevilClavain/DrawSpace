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

#include "exceptions.h"
#include "logger_service.h"

#include "logsink.h"
#include "logconf.h"


using namespace mage::core::services;

void LoggerSharing::createLogger(const std::string& p_id)
{
	if (!m_loggers.count(p_id))
	{
		m_loggers[p_id] = std::make_unique<mage::core::logger::Sink>(p_id, mage::core::logger::Configuration::getInstance());
	}
	else
	{
		_EXCEPTION("logger already created");
	}
}

mage::core::logger::Sink& LoggerSharing::getLogger(const std::string& p_id)
{
	if (m_loggers.count(p_id) > 0)
	{
		return *m_loggers.at(p_id);
	}
	else
	{
		_EXCEPTION("Unknow logger");
	}
}