/***************************************************************************
*                                                                          *
* DrawSpace Rendering engine                                               *
* Emmanuel Chaumont Copyright (c) 2013-2014                                *
*                                                                          *
* This file is part of DrawSpace.                                          *
*                                                                          *
*    DrawSpace is free software: you can redistribute it and/or modify     *
*    it under the terms of the GNU General Public License as published by  *
*    the Free Software Foundation, either version 3 of the License, or     *
*    (at your option) any later version.                                   *
*                                                                          *
*    DrawSpace is distributed in the hope that it will be useful,          *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*    GNU General Public License for more details.                          *
*                                                                          *
*    You should have received a copy of the GNU General Public License     *
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    *
*                                                                          *
***************************************************************************/

#include "widget.h"

using namespace DrawSpace;
using namespace DrawSpace::Gui;

Widget::Widget( const dsstring& p_name ) : m_name( p_name ), m_parent( NULL )
{
}

Widget::~Widget( void )
{
}

void Widget::AddChild( Widget* p_widget )
{
	m_children.push_back( p_widget );
}

void Widget::MouseMoveInput( long p_xm, long p_ym, long p_dx, long p_dy )
{

}

void Widget::MouseLeftButtonDownInput( long p_xm, long p_ym )
{

}

void Widget::MouseLeftButtonUpInput( long p_xm, long p_ym )
{


}

void Widget::MouseRightButtonDownInput( long p_xm, long p_ym )
{

}

void Widget::MouseRightButtonUpInput( long p_xm, long p_ym )
{

}

