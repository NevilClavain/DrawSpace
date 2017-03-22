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

#ifndef _STATUSBARTIMER_H_
#define _STATUSBARTIMER_H_

#include "drawspace.h"

class StatusBarTimer
{
protected:
    typedef DrawSpace::Core::CallBack<StatusBarTimer, void, DrawSpace::Utils::Timer*>            TimerCb;

    dsstring                            m_layout;
    dsstring                            m_widget_id;

    TimerCb*                            m_statusbar_timer_cb;
    DrawSpace::Utils::Timer*            m_statusbar_timer;
    DrawSpace::Interface::Renderer*     m_renderer;

    void on_statusbar_timer( DrawSpace::Utils::Timer* p_timer );

public:

    StatusBarTimer( const dsstring& p_layout, const dsstring& p_widget_id );
    ~StatusBarTimer( void );

    void Init( DrawSpace::Utils::TimeManager& p_tm );

    void Print( const dsstring& p_msg );


};

#endif