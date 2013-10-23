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

#include "text_widget.h"

using namespace DrawSpace;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Core;

TextWidget::TextWidget( const dsstring& p_name, long p_virtual_width, long p_virtual_height, DrawSpace::Core::Font* p_font, bool p_backgroundimage, Widget* p_parentwidget ) : 
Widget( p_name, p_virtual_width, p_virtual_height, p_parentwidget )
{	
    m_pass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( m_name + "/pass" ) );

    m_image = _DRAWSPACE_NEW_( Image, Image( m_real_width, m_real_height ) );   
    m_image->SetTexture( m_pass->GetTargetTexture(), 0 );

    m_pass->GetRenderingQueue()->EnableDepthClearing( false );
    m_pass->GetRenderingQueue()->EnableTargetClearing( true );
    m_pass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0 );

    if( p_backgroundimage )
    {
        m_backgroundimage = _DRAWSPACE_NEW_( Image, Image( (long)DRAWSPACE_GUI_WIDTH, (long)DRAWSPACE_GUI_HEIGHT ) );
        m_pass->GetRenderingQueue()->Add( m_backgroundimage );
    }
    else
    {
        m_backgroundimage = NULL;
    }

    m_text = _DRAWSPACE_NEW_( Text, Text( p_font ) );
    m_pass->GetRenderingQueue()->Add( m_text );
}

TextWidget::~TextWidget( void )
{
}

void TextWidget::SetVirtualTranslation( long p_x ,long p_y )
{
    Widget::SetVirtualTranslation( p_x, p_y );
    //m_image->SetVirtualTranslation( p_x, p_y );
    m_image->SetTranslation( m_real_posx, m_real_posy );

}

void TextWidget::SetBackgroundTexture( Core::Texture* p_backgroundtexture, long p_stage )
{
    if( m_backgroundimage )
    {
        m_backgroundimage->SetTexture( p_backgroundtexture, p_stage );
    }
}

Core::Fx* TextWidget::GetImageFx( void )
{
    return m_image->GetFx();
}

Core::Fx* TextWidget::GetBackGroundImageFx( void )
{
    if( m_backgroundimage )
    {
        return m_backgroundimage->GetFx();
    }
    return NULL;
}

Core::Texture* TextWidget::GetBackGroundImageTexture( long p_stage )
{
    if( m_backgroundimage )
    {
        return m_backgroundimage->GetTexture( p_stage );
    }
    return NULL;
}

Core::Fx* TextWidget::GetTextFx( void )
{
    return m_text->GetFx();
}

void TextWidget::SetText( long p_x, long p_y, long p_height, const dsstring& p_text, unsigned char p_flag )
{
    m_text->SetText( p_x, p_y, p_height, p_text, p_flag );
}

void TextWidget::Draw( void )
{
    Widget::Draw();

    m_pass->GetRenderingQueue()->Draw();
}

void TextWidget::RegisterToPass( Pass* p_pass )
{
    p_pass->GetRenderingQueue()->Add( m_image );
}

bool TextWidget::LoadAssets( void )
{
    if( m_backgroundimage && false == m_backgroundimage->LoadAssets() )
    {
        return false;
    }
    if( false == m_image->LoadAssets() )
    {
        return false;
    }
    if( false == m_text->LoadAssets() )
    {
        return false;
    }
    return true;
}

void TextWidget::SetPassTargetClearingColor( unsigned char p_r, unsigned char p_g, unsigned char p_b )
{
    m_pass->GetRenderingQueue()->SetTargetClearingColor( p_r, p_g, p_b );
}