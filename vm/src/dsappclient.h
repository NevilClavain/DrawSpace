/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2022                  
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
/* -*-LIC_END-*- */


#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>

class dsAppClient : public DrawSpace::App
{
protected:
    typedef DrawSpace::Core::CallBack<dsAppClient, void, bool>                                 MouseCircularModeupdateCallback;
    typedef DrawSpace::Core::CallBack<dsAppClient, void, bool>                                 MouseVisibleCallback;

    typedef DrawSpace::Core::CallBack<dsAppClient, void, int>                                  CloseAppCallback;

    MouseCircularModeupdateCallback*            m_mouse_circularmode_update_cb;
    MouseVisibleCallback*                       m_mouse_visible_cb;

    CloseAppCallback*                           m_close_app_cb;
    static dsAppClient*                         m_instance;

    DrawSpace::Interface::Module::Service*      m_mainloopservice;
    bool                                        m_mousecursor_visible;

    void on_mouse_visible( bool p_state );
    void on_mousecircularmode_update( bool p_state );
    void on_closeapp( int p_code );

    dsAppClient( void );

public:

    ~dsAppClient( void );

    static dsAppClient* GetInstance( void )
    {
        if( m_instance == NULL )
        {
            m_instance = new dsAppClient;
        }

        DrawSpace::App::m_base_instance = m_instance;
        return m_instance;
    }

    virtual void        OnRenderFrame( void );
    virtual bool        OnIdleAppInit( void );
    virtual void        OnAppInit( void );
    virtual void        OnClose( void );
    virtual void        OnKeyPress( long p_key ) ;
    virtual void        OnEndKeyPress( long p_key );
    virtual void        OnKeyPulse( long p_key );
    virtual void        OnChar( long p_char, long p_scan );
    virtual void        OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy );
    virtual void        OnMouseWheel( long p_delta );
    virtual void        OnMouseLeftButtonDown( long p_xm, long p_ym );
    virtual void        OnMouseLeftButtonUp( long p_xm, long p_ym );
    virtual void        OnMouseRightButtonDown( long p_xm, long p_ym );
    virtual void        OnMouseRightButtonUp( long p_xm, long p_ym );
    virtual void        OnAppEvent( WPARAM p_wParam, LPARAM p_lParam );
};

#endif

