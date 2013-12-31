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

#ifndef _TEXT_H_
#define _TEXT_H_

#include "renderingnode.h"
#include "font.h"

namespace DrawSpace
{
class Text : public Core::RenderingNode
{
public:
    static const int HorizontalCentering    = 0x01;
    static const int VerticalCentering      = 0x02;

protected:
    dsstring                                m_text;
    dsreal                                  m_height;
    Core::Font*                             m_font;
    dsreal                                  m_x, m_y;
    Interface::Renderer::Characteristics    m_rc;

public:
    Text( Core::Font* p_font );
    virtual ~Text( void );

    bool Initialize( void );
    
    void OnDraw( void );
    void SetText( long p_x, long p_y, long p_height, const dsstring& p_text, unsigned char p_flag = 0 );
};
}


#endif