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

#pragma once

#include <windows.h>
#include <string>
#include "logconf.h"

#define _MAGE_TRACE( _logger, _message ) _logger.logIt( mage::core::logger::Sink::Level::LEVEL_TRACE, std::string(__FUNCTION__) + std::string( " " ) + _message );
#define _MAGE_DEBUG( _logger, _message ) _logger.logIt( mage::core::logger::Sink::Level::LEVEL_DEBUG, std::string(__FUNCTION__) + std::string( " " ) + _message );
#define _MAGE_WARN( _logger, _message )  _logger.logIt( mage::core::logger::Sink::Level::LEVEL_WARN, std::string(__FUNCTION__) + std::string( " " ) + _message );
#define _MAGE_ERROR( _logger, _message ) _logger.logIt( mage::core::logger::Sink::Level::LEVEL_ERROR, std::string(__FUNCTION__) + std::string( " " ) + _message );
#define _MAGE_FATAL( _logger, _message ) _logger.logIt( mage::core::logger::Sink::Level::LEVEL_FATAL, std::string(__FUNCTION__) + std::string( " " ) + _message );

std::string operator<< (const std::string& p_s1, const std::string& p_s2);
std::string operator<< (const std::string& p_s1, const char* p_s2);
std::string operator<< (const char* p_s1, const std::string& p_s2);
std::string operator<< (const std::string& p_s1, int p_s2);
std::string operator<< (const std::string& p_s1, size_t p_s2);
std::string operator<< (const std::string& p_s1, void* p_s2);
std::string operator<< (const std::string& p_s1, bool p_s2);
std::string operator<< (const std::string& p_s1, DWORD p_s2);
std::string operator<< (const std::string& p_s1, float p_s2);
std::string operator<< (const std::string& p_s1, double p_s2);

