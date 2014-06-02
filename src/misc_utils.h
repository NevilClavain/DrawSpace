/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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
#include "text_widget.h"
#include "spacebox.h"
#include "pimanager.h"

namespace DrawSpace
{
namespace Utils
{

Gui::TextWidget* BuildText( DrawSpace::Core::Font* p_font, long p_width, long p_height, const Vector& p_color, const dsstring& p_name );

bool LoadFontImportPlugin( const dsstring& p_path, const dsstring& p_pluginalias );
Interface::FontImport* InstanciateFontImportFromPlugin( const dsstring& p_pluginalias );

bool LoadMesheImportPlugin( const dsstring& p_path, const dsstring& p_pluginalias );
Interface::MesheImport* InstanciateMesheImportFromPlugin( const dsstring& p_pluginalias );

void BuildSpaceboxFx( DrawSpace::Spacebox* p_spacebox, const dsstring& p_passname );

}
}

#endif
