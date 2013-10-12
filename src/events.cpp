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

#include "events.h"

using namespace DrawSpace;

MouseInputsProvider::MouseInputsProvider( void )
{


}

MouseInputsProvider::~MouseInputsProvider( void )
{


}

void MouseInputsProvider::RegisterWidget( Gui::Widget* p_widget )
{
    m_widgets.push_back( p_widget );
}



SystemMouseInputProvider::SystemMouseInputProvider( void )
{


}

SystemMouseInputProvider::~SystemMouseInputProvider( void )
{


}

void SystemMouseInputProvider::OnMouseMove( long p_xm, long p_ym )
{
    for( size_t i = 0;i < m_widgets.size(); i++ )
    {
        m_widgets[i]->MouseMoveInput( p_xm, p_ym );
    }
}

void SystemMouseInputProvider::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    for( size_t i = 0;i < m_widgets.size(); i++ )
    {
        m_widgets[i]->MouseLeftButtonDownInput( p_xm, p_ym );
    }
}

void SystemMouseInputProvider::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    for( size_t i = 0;i < m_widgets.size(); i++ )
    {
        m_widgets[i]->MouseLeftButtonUpInput( p_xm, p_ym );
    }
}

void SystemMouseInputProvider::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    for( size_t i = 0;i < m_widgets.size(); i++ )
    {
        m_widgets[i]->MouseRightButtonDownInput( p_xm, p_ym );
    }
}

void SystemMouseInputProvider::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    for( size_t i = 0;i < m_widgets.size(); i++ )
    {
        m_widgets[i]->MouseRightButtonUpInput( p_xm, p_ym );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

ImageMouseInputProvider::ImageMouseInputProvider( void ) : m_image( NULL )
{


}

ImageMouseInputProvider::~ImageMouseInputProvider( void )
{


}

void ImageMouseInputProvider::RegisterImage( Image* p_image )
{
    m_image = p_image;
}

void ImageMouseInputProvider::OnMouseMove( long p_xm, long p_ym )
{
    if( !m_image )
    {
        return;
    }

    long x, y;
    m_image->GetVirtualTranslation( x, y );
    for( size_t i = 0;i < m_widgets.size(); i++ )
    {
        m_widgets[i]->MouseMoveInputVirtualCoords( x, y );
    }
}

void ImageMouseInputProvider::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    for( size_t i = 0;i < m_widgets.size(); i++ )
    {
        m_widgets[i]->MouseLeftButtonDownInput( p_xm, p_ym );
    }
}

void ImageMouseInputProvider::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    for( size_t i = 0;i < m_widgets.size(); i++ )
    {
        m_widgets[i]->MouseLeftButtonUpInput( p_xm, p_ym );
    }
}

void ImageMouseInputProvider::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    for( size_t i = 0;i < m_widgets.size(); i++ )
    {
        m_widgets[i]->MouseRightButtonDownInput( p_xm, p_ym );
    }
}

void ImageMouseInputProvider::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    for( size_t i = 0;i < m_widgets.size(); i++ )
    {
        m_widgets[i]->MouseRightButtonUpInput( p_xm, p_ym );
    }
}
