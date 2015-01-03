/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "widget.h"

using namespace DrawSpace;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Interface;

Widget::Widget( const dsstring& p_name, long p_virtual_width, long p_virtual_height, Widget* p_parentwidget ) : 
m_name( p_name ), 
m_virtual_width( p_virtual_width ), 
m_virtual_height( p_virtual_height ), 
m_virtual_posx( 0 ),
m_virtual_posy( 0 ),
m_parent( p_parentwidget ),
m_flag_cursor_in( false ),
m_mousein_handler( NULL ),
m_mouseout_handler( NULL ),
m_mouserightbuttonup_handler( NULL ),
m_mouserightbuttondown_handler( NULL ),
m_mouseleftbuttonup_handler( NULL ),
m_mouseleftbuttondown_handler( NULL ),
m_drawingstate( true )
{
    Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->GetRenderCharacteristics( m_rc );

    if( NULL == m_parent )
    {
        m_real_width = ( (dsreal)p_virtual_width / (dsreal)DRAWSPACE_GUI_WIDTH ) * m_rc.width_viewport;
        m_real_height = ( (dsreal)p_virtual_height / (dsreal)DRAWSPACE_GUI_HEIGHT ) * m_rc.height_viewport;
    }
    else
    {
        m_real_width = ( (dsreal)p_virtual_width / (dsreal)DRAWSPACE_GUI_WIDTH ) * m_parent->m_real_width;
        m_real_height = ( (dsreal)p_virtual_height / (dsreal)DRAWSPACE_GUI_HEIGHT ) * m_parent->m_real_height;
    }

    if( m_parent )
    {
        m_parent->m_children.push_back( this );
    }
}

Widget::~Widget( void )
{
}


void Widget::GetName( dsstring& p_name )
{
    p_name = m_name;
}

void Widget::Transform( void )
{
    if( NULL == m_parent )
    {
        m_real_posx = ( ( ( (dsreal)m_virtual_posx / (dsreal)DRAWSPACE_GUI_WIDTH ) - 0.5 ) /*/ 0.5 */) * ( m_rc.width_viewport /*/ 2.0*/ );
        m_real_posy = ( ( 0.5 - ( (dsreal)m_virtual_posy / (dsreal)DRAWSPACE_GUI_HEIGHT ) ) /*/ 0.5 */ ) * ( m_rc.height_viewport /*/ 2.0*/ );
    }
    else
    {
        m_real_posx = ( ( ( ( (dsreal)m_virtual_posx / (dsreal)DRAWSPACE_GUI_WIDTH ) - 0.5 ) /*/ 0.5 */) * ( m_parent->m_real_width /*/ 2.0*/ ) ) + m_parent->m_real_posx;
        m_real_posy = ( ( ( 0.5 - ( (dsreal)m_virtual_posy / (dsreal)DRAWSPACE_GUI_HEIGHT ) ) /*/ 0.5 */ ) * ( m_parent->m_real_height /*/ 2.0*/ ) ) + m_parent->m_real_posy;
    }

    for( size_t i = 0; i < m_children.size(); i++ )
    {
        m_children[i]->Transform();
    }
}

void Widget::SetVirtualTranslation( long p_x, long p_y )
{
    m_virtual_posx = p_x;
    m_virtual_posy = p_y;
}

void Widget::manage_mouse_move( dsreal p_xm, dsreal p_ym )
{
    if( m_real_posx - m_real_width / 2.0 < p_xm && p_xm < m_real_posx + m_real_width / 2.0 &&
        m_real_posy - m_real_height / 2.0 < p_ym && p_ym < m_real_posy + m_real_height / 2.0 )
    {
        if( !m_flag_cursor_in )
        {
            if( m_mousein_handler )
            {
                (*m_mousein_handler)( this );
            }
        }
        m_flag_cursor_in = true;
    }
    else
    {
        if( m_flag_cursor_in )
        {
            if( m_mouseout_handler )
            {
                (*m_mouseout_handler)( this );
            }
        }
        m_flag_cursor_in = false;
    }
}

void Widget::MouseMoveInput( long p_xm, long p_ym )
{
    for( size_t i = 0; i < m_children.size(); i++ )
    {
        m_children[i]->MouseMoveInput( p_xm, p_ym );
    }

    dsreal real_x, real_y;
    real_x = ( ( (dsreal)p_xm / m_rc.width_resol ) - 0.5 ) * m_rc.width_viewport;
    real_y = ( 0.5 - ( (dsreal)p_ym / m_rc.height_resol ) ) * m_rc.height_viewport;

    manage_mouse_move( real_x, real_y );
}

void Widget::MouseMoveInputVirtualCoords( long p_xm, long p_ym )
{
    for( size_t i = 0; i < m_children.size(); i++ )
    {
        m_children[i]->MouseMoveInputVirtualCoords( p_xm, p_ym );
    }

    dsreal real_x, real_y;
    real_x = ( ( (dsreal)p_xm / DRAWSPACE_GUI_WIDTH ) - 0.5 ) * m_rc.width_viewport;
    real_y = ( 0.5 - ( (dsreal)p_ym / DRAWSPACE_GUI_HEIGHT ) ) * m_rc.height_viewport;

    manage_mouse_move( real_x, real_y );
}

void Widget::MouseLeftButtonDownInput( long p_xm, long p_ym )
{
    for( size_t i = 0; i < m_children.size(); i++ )
    {
        m_children[i]->MouseLeftButtonDownInput( p_xm, p_ym );
    }

    if( m_flag_cursor_in && m_mouseleftbuttondown_handler )
    {
        (*m_mouseleftbuttondown_handler)( this );
    }
}

void Widget::MouseLeftButtonUpInput( long p_xm, long p_ym )
{
    for( size_t i = 0; i < m_children.size(); i++ )
    {
        m_children[i]->MouseLeftButtonUpInput( p_xm, p_ym );
    }

    if( m_flag_cursor_in && m_mouseleftbuttonup_handler )
    {
        (*m_mouseleftbuttonup_handler)( this );
    }
}

void Widget::MouseRightButtonDownInput( long p_xm, long p_ym )
{
    for( size_t i = 0; i < m_children.size(); i++ )
    {
        m_children[i]->MouseRightButtonDownInput( p_xm, p_ym );
    }

    if( m_flag_cursor_in && m_mouserightbuttondown_handler )
    {
        (*m_mouserightbuttondown_handler)( this );
    }
}

void Widget::MouseRightButtonUpInput( long p_xm, long p_ym )
{
    for( size_t i = 0; i < m_children.size(); i++ )
    {
        m_children[i]->MouseRightButtonUpInput( p_xm, p_ym );
    }

    if( m_flag_cursor_in && m_mouserightbuttonup_handler )
    {
        (*m_mouserightbuttonup_handler)( this );
    }
}

void Widget::Draw( void )
{
    for( size_t i = 0; i < m_children.size(); i++ )
    {
        m_children[i]->Draw();
    }
}

void Widget::RegisterMouseInEventHandler( EventHandler p_handler )
{
    m_mousein_handler = p_handler;
}

void Widget::RegisterMouseOutEventHandler( EventHandler p_handler )
{
    m_mouseout_handler = p_handler;
}

void Widget::RegisterMouseRightButtonUpEventHandler( EventHandler p_handler )
{
    m_mouserightbuttonup_handler = p_handler;
}

void Widget::RegisterMouseRightButtonDownEventHandler( EventHandler p_handler )
{
    m_mouserightbuttondown_handler = p_handler;
}

void Widget::RegisterMouseLeftButtonUpEventHandler( EventHandler p_handler )
{
    m_mouseleftbuttonup_handler = p_handler;
}

void Widget::RegisterMouseLeftButtonDownEventHandler( EventHandler p_handler )
{
    m_mouseleftbuttondown_handler = p_handler;
}

void Widget::SetDrawingState( bool p_state )
{
    m_drawingstate = p_state;

    for( size_t i = 0; i < m_children.size(); i++ )
    {
        m_children[i]->SetDrawingState( p_state );
    }
}

void Widget::GetScreenPos( int& p_outx, int& p_outy )
{
    /*
    dsreal outx = (int)( m_real_posx + ( m_rc.width_viewport / 2.0 ) ) * m_rc.width_resol;
    dsreal outy = (int)( m_real_posy + ( m_rc.height_viewport / 2.0 ) ) * m_rc.height_resol;
    */

    dsreal t_x = m_real_posx + ( m_rc.width_viewport / 2.0 );
    dsreal t_y = m_rc.height_viewport - ( m_real_posy + ( m_rc.height_viewport / 2.0 ) );

    dsreal resol_x = ( t_x / m_rc.width_viewport ) * m_rc.width_resol;
    dsreal resol_y = ( t_y / m_rc.height_viewport ) * m_rc.height_resol;

    p_outx = (int)resol_x;
    p_outy = (int)resol_y;
}
