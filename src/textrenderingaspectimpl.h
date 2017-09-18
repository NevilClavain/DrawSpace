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

#ifndef _TEXTRENDERINGASPECTIMPL_H_
#define _TEXTRENDERINGASPECTIMPL_H_

#include "renderingaspectimpl.h"
#include "plugin.h"
#include "renderer.h"

namespace DrawSpace
{
namespace Core
{
class TextRenderingAspectImpl : public RenderingAspectImpl
{
protected:
    DrawSpace::Interface::Renderer* m_renderer;

public:
    struct TextDisplay sealed
    {
        dsstring        m_text;
        int             m_posx;
        int             m_posy;

        unsigned char   m_r;
        unsigned char   m_g;
        unsigned char   m_b;  

        TextDisplay( int p_posx, int p_posy, unsigned char p_r, unsigned char p_g, unsigned char p_b, const dsstring& p_text ) :
            m_r( p_r ),
            m_g( p_g ),
            m_b( p_b ),
            m_posx( p_posx ),
            m_posy( p_posy ),
            m_text( p_text )
        {
        }
    };
    
protected:
    virtual void draw( RenderingAspect* p_renderingaspect );

public:
    TextRenderingAspectImpl( void );
};

}
}

#endif