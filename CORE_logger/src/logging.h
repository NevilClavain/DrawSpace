/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of DrawSpace.
*
*    DrawSpace is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    DrawSpace is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#include <windows.h>
#include "ds_types.h"
#include "logconf.h"

#define _DSTRACE( _logger, _message ) _logger.LogIt( DrawSpace::Logger::Sink::Level::LEVEL_TRACE, dsstring(__FUNCTION__) + dsstring( " " ) + _message );
#define _DSDEBUG( _logger, _message ) _logger.LogIt( DrawSpace::Logger::Sink::Level::LEVEL_DEBUG, dsstring(__FUNCTION__) + dsstring( " " ) + _message );
#define _DSWARN( _logger, _message )  _logger.LogIt( DrawSpace::Logger::Sink::Level::LEVEL_WARN, dsstring(__FUNCTION__) + dsstring( " " ) + _message );
#define _DSERROR( _logger, _message ) _logger.LogIt( DrawSpace::Logger::Sink::Level::LEVEL_ERROR, dsstring(__FUNCTION__) + dsstring( " " ) + _message );
#define _DSFATAL( _logger, _message ) _logger.LogIt( DrawSpace::Logger::Sink::Level::LEVEL_FATAL, dsstring(__FUNCTION__) + dsstring( " " ) + _message );

dsstring operator<< (const dsstring& p_s1, const dsstring& p_s2);
dsstring operator<< (const dsstring& p_s1, const char* p_s2);
dsstring operator<< (const char* p_s1, const dsstring& p_s2);
dsstring operator<< (const dsstring& p_s1, int p_s2);
dsstring operator<< (const dsstring& p_s1, size_t p_s2);
dsstring operator<< (const dsstring& p_s1, void* p_s2);
dsstring operator<< (const dsstring& p_s1, bool p_s2);
dsstring operator<< (const dsstring& p_s1, DWORD p_s2);
dsstring operator<< (const dsstring& p_s1, float p_s2);
dsstring operator<< (const dsstring& p_s1, dsreal p_s2);

