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

#include "misc_utils.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;


long DrawSpace::Utils::StringToInt( const dsstring& p_value )
{
    return atoi( p_value.c_str() );
}

dsreal DrawSpace::Utils::StringToReal( const dsstring& p_value )
{
    return atof( p_value.c_str() );
}


dswstring DrawSpace::Utils::String2WString( const dsstring& p_s )
{
    int len;
    int slength = (int)p_s.length() + 1;
    len = MultiByteToWideChar( CP_ACP, 0, p_s.c_str(), slength, 0, 0 ); 
    dswstring r( len, L'\0' );
    MultiByteToWideChar( CP_ACP, 0, p_s.c_str(), slength, &r[0], len );
    return r;
}

dsstring DrawSpace::Utils::WString2String( const dswstring& p_s )
{
    int len;
    int slength = (int)p_s.length() + 1;
    len = WideCharToMultiByte( CP_ACP, 0, p_s.c_str(), slength, 0, 0, 0, 0 ); 
    dsstring r( len, '\0' );
    WideCharToMultiByte( CP_ACP, 0, p_s.c_str(), slength, &r[0], len, 0, 0 ); 
    return r;
}
