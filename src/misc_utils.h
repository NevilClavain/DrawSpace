/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _MISC_UTILS_H_
#define _MISC_UTILS_H_

#include "font.h"

#include "spacebox.h"
#include "pimanager.h"

namespace DrawSpace
{
namespace Utils
{
    bool LoadFontImportPlugin( const dsstring& p_path, const dsstring& p_pluginalias );
    Interface::FontImport* InstanciateFontImportFromPlugin( const dsstring& p_pluginalias );

    void BuildSpaceboxFx( DrawSpace::Spacebox* p_spacebox, Pass* p_pass );

    long StringToInt( const dsstring& p_value );
    dsreal StringToReal( const dsstring& p_value );

    void IntToString( long p_value, dsstring& p_str );
    void RealToString( dsreal p_value, dsstring& p_str );

    dswstring   String2WString( const dsstring& p_s );
    dsstring    WString2String( const dswstring& p_s );
}
}

#endif
