/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#ifndef _CBFGFONT_H_
#define _CBFGFONT_H_

#include "fontimport.h"
#include "parser.h"

namespace DrawSpace
{
namespace Utils
{
class CBFGFontImport : public DrawSpace::Interface::FontImport, public DrawSpace::Utils::Parser
{
protected:

    long                    m_texture_width;
    long                    m_texture_height;
    long                    m_cell_width;
    long                    m_cell_height;
    char                    m_start_char;

    DrawSpace::Core::Font*  m_font;

    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );
public:
    CBFGFontImport( void );
    virtual ~CBFGFontImport( void );

    virtual bool LoadFromFile( const dsstring& p_metricsfilepath, DrawSpace::Core::Font* p_font );
};
}
}

#endif
