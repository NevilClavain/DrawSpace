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
#include <string>
#include <map>
#include <vector>
#include <set>
#include <exception>
#include <memory>
#include <unordered_map>
#include <typeinfo>
#include <algorithm>
#include <functional>
#include <array>
#include <sstream>
#include <iterator>
#include <mutex>
#include <limits>

#pragma warning( disable : 4231 4996 4311 4800 4244 4305 4477 )
#define _DRAWSPACE_ENABLE_TRACES_

using dsstring		= std::string;
using dswstring		= std::wstring;
using dsreal		= double;
using dstime		= __time64_t;
using dsexception	= std::exception;


dsstring operator<< ( dsstring& p_s1, dsstring& p_s2 );
dsstring operator<< ( dsstring& p_s1, const char* p_s2 );
dsstring operator<< ( const char* p_s1, dsstring& p_s2 );
dsstring operator<< ( dsstring& p_s1, int p_s2 );
dsstring operator<< ( dsstring& p_s1, size_t p_s2 );
dsstring operator<< ( dsstring& p_s1, void* p_s2 );
dsstring operator<< ( dsstring& p_s1, bool p_s2 );
dsstring operator<< ( dsstring& p_s1, DWORD p_s2 );
dsstring operator<< ( dsstring& p_s1, float p_s2 );
dsstring operator<< ( dsstring& p_s1, dsreal p_s2 );
