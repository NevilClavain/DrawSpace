
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

#pragma once

#include <string>
#include <exception>

namespace renderMe
{
    namespace core
    {
        static void throwException(const std::string& p_msg, const std::string& p_func)
        {
            const auto message{ p_msg + std::string(" @ ") + std::string(p_func) };
            throw std::exception(message.c_str());
        }
    }
}

#define _EXCEPTION( _msg ) renderMe::core::throwException(_msg, __FUNCTION__);

