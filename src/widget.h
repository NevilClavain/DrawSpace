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

#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "drawspace_commons.h"
#include "image.h"
#include "text.h"
#include "callback.h"
#include "renderingnode.h"
#include "renderer.h"
#include "plugin.h"
#include "callback.h"

namespace DrawSpace
{
namespace Gui
{
class Widget
{
public:

    typedef Core::BaseCallback<void, Widget*>*          EventHandler;

    EventHandler                                        m_mousein_handler;
    EventHandler                                        m_mouseout_handler;
    EventHandler                                        m_mouserightbuttonup_handler;
    EventHandler                                        m_mouserightbuttondown_handler;
    EventHandler                                        m_mouseleftbuttonup_handler;
    EventHandler                                        m_mouseleftbuttondown_handler;

protected:
    dsstring                                            m_name;

    Widget*                                             m_parent;
    std::vector<Widget*>                                m_children;

    long                                                m_virtual_width;
    long                                                m_virtual_height;

    Interface::Renderer::Characteristics                m_rc;

    dsreal                                              m_real_width;
    dsreal                                              m_real_height;

    long                                                m_virtual_posx;
    long                                                m_virtual_posy;

    dsreal                                              m_real_posx;
    dsreal                                              m_real_posy;

    bool                                                m_flag_cursor_in;

    bool                                                m_drawingstate;

    void                                                manage_mouse_move( dsreal p_xm, dsreal p_ym );

public:
    Widget( const dsstring& p_name, long p_virtual_width, long p_virtual_height, Widget* p_parentwidget );
    virtual ~Widget( void );

    virtual void SetVirtualTranslation( long p_x, long p_y );
    virtual void Transform( void );
    virtual void Draw( void );

    virtual void MouseMoveInput( long p_xm, long p_ym );
    virtual void MouseMoveInputVirtualCoords( long p_xm, long p_ym );
    virtual void MouseLeftButtonDownInput( long p_xm, long p_ym );
    virtual void MouseLeftButtonUpInput( long p_xm, long p_ym );
    virtual void MouseRightButtonDownInput( long p_xm, long p_ym );
    virtual void MouseRightButtonUpInput( long p_xm, long p_ym );

    virtual void GetName( dsstring& p_name );

    virtual void RegisterMouseInEventHandler( EventHandler p_handler );
    virtual void RegisterMouseOutEventHandler( EventHandler p_handler );
    virtual void RegisterMouseRightButtonUpEventHandler( EventHandler p_handler );
    virtual void RegisterMouseRightButtonDownEventHandler( EventHandler p_handler );
    virtual void RegisterMouseLeftButtonUpEventHandler( EventHandler p_handler );
    virtual void RegisterMouseLeftButtonDownEventHandler( EventHandler p_handler );

    virtual void SetDrawingState( bool p_state );

    virtual void GetScreenPos( int& p_outx, int& p_outy );


};
}
}

#endif
