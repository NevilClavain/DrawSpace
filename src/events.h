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

/*
following this schema :

MouseInputEventProvider::registerWidget( Widget* )


SystemMouseInputEventSource : public MouseInputEventSource
ImageMouseInputEventSource : public MouseInputEventSource

dsApp::registerMouseInputEventSource( MouseInputEventtSource* )



*/

#ifndef _EVENTS_H_
#define _EVENTS_H_

#include "drawspace_commons.h"
#include "widget.h"
#include "image.h"

namespace DrawSpace
{
class MouseInputsProvider
{
protected:

    std::vector<Gui::Widget*>   m_widgets;

public:
    MouseInputsProvider( void );
    virtual ~MouseInputsProvider( void );

    void RegisterWidget( Gui::Widget* p_widget );

    virtual void OnMouseMove( long p_xm, long p_ym ) = 0;

    virtual void OnMouseLeftButtonDown( long p_xm, long p_ym ) = 0;
    virtual void OnMouseLeftButtonUp( long p_xm, long p_ym ) = 0;

    virtual void OnMouseRightButtonDown( long p_xm, long p_ym ) = 0;
    virtual void OnMouseRightButtonUp( long p_xm, long p_ym ) = 0;

};



class SystemMouseInputProvider : public MouseInputsProvider
{
protected:

public:
    SystemMouseInputProvider( void );
    virtual ~SystemMouseInputProvider( void );

    virtual void OnMouseMove( long p_xm, long p_ym );

    virtual void OnMouseLeftButtonDown( long p_xm, long p_ym );
    virtual void OnMouseLeftButtonUp( long p_xm, long p_ym );

    virtual void OnMouseRightButtonDown( long p_xm, long p_ym );
    virtual void OnMouseRightButtonUp( long p_xm, long p_ym );

};

class ImageMouseInputProvider : public MouseInputsProvider
{
protected:
    Image*          m_image;

public:
    ImageMouseInputProvider( void );
    virtual ~ImageMouseInputProvider( void );

    virtual void RegisterImage( Image* p_image );

    virtual void OnMouseMove( long p_xm, long p_ym );

    virtual void OnMouseLeftButtonDown( long p_xm, long p_ym );
    virtual void OnMouseLeftButtonUp( long p_xm, long p_ym );

    virtual void OnMouseRightButtonDown( long p_xm, long p_ym );
    virtual void OnMouseRightButtonUp( long p_xm, long p_ym );

};

}

#endif
