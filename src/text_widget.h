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

#ifndef _TEXT_WIDGET_H_
#define _TEXT_WIDGET_H_

#include "widget.h"
#include "pass.h"

namespace DrawSpace
{
namespace Gui
{
class TextWidget : public Widget
{
protected:

    // image de fond
    Image*              m_backgroundimage;

    // texte
    DrawSpace::Text*    m_text;

    // image finale (representation concrete du widget)
    IntermediatePass*   m_pass;
    Image*              m_image;
    

public:
    TextWidget( const dsstring& p_name, long p_virtual_width, long p_virtual_height, DrawSpace::Core::Font* p_font, bool p_backgroundimage, Widget* p_parentwidget );
    virtual ~TextWidget( void );

    virtual void SetVirtualTranslation( long p_x ,long p_y );

    virtual Core::Fx* GetImageFx( void );
    virtual Core::Fx* GetBackGroundImageFx( void );
    virtual Core::Texture* GetBackGroundImageTexture( long p_stage );
    virtual Core::Fx* GetTextFx( void );

    virtual void SetBackgroundTexture( Core::Texture* p_backgroundtexture, long p_stage );

    virtual void SetText( long p_x, long p_y, long p_height, const dsstring& p_text, unsigned char p_flag = 0 );

    virtual void Draw( void );
    virtual void RegisterToPass( Pass* p_pass );
    virtual bool LoadAssets( void );

    virtual void SetPassTargetClearingColor( unsigned char p_r, unsigned char p_g, unsigned char p_b );

};
}
}

#endif