/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#ifndef _MISC_UTILS_H_
#define _MISC_UTILS_H_

#include "drawspace_commons.h"


namespace DrawSpace
{
namespace Utils
{

    long		StringToInt( const dsstring& p_value );
    dsreal		StringToReal( const dsstring& p_value );

    dswstring   String2WString( const dsstring& p_s );
    dsstring    WString2String( const dswstring& p_s );

	template <class Container>
	void		SplitString(const dsstring& str, Container& cont, char delim = ' ')
	{
		std::stringstream ss(str);
		std::string token;
		while (std::getline(ss, token, delim))
		{
			cont.push_back(token);
		}
	}
}
}

#endif
