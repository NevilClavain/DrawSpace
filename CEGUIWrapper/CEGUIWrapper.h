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

#include <CEGUI\CEGUI.h>
#include "drawspace_commons.h"
#include "callback.h"

#ifndef _CEGUIWRAPPER_H_
#define _CEGUIWRAPPER_H_

class CEGUIWrapper
{
protected:

    bool                                                m_ready;
    std::map<int, CEGUI::Window*>                       m_ceguiWindowTable;
    std::map<dsstring, CEGUI::Window*>                  m_ceguiLayoutTable;

    DrawSpace::Core::BaseCallback<void, dsstring>*      m_pushbuttoneventclicked_handler;

    bool            on_PushButton_EventClicked(const CEGUI::EventArgs& p_evt );

public:
    CEGUIWrapper( void );
    ~CEGUIWrapper( void );

    
    void SetReady( void );
    void RenderGUI( void );

    void SetResourcesRootDirectory( const dsstring& p_path );

    void OnMouseMove( float p_xm, float p_ym, float p_dx, float p_dy );
    void OnMouseLeftButtonDown( void );
    void OnMouseLeftButtonUp( void );
    void OnMouseRightButtonDown( void );
    void OnMouseRightButtonUp( void );
    void OnKeyDown( long p_key );
    void OnKeyUp( long p_key );
    void OnChar( long p_key );

    void LoadLayoutFromFile( const dsstring& p_layout_path, const dsstring& p_scheme_path );

    void SetLayout( const dsstring& p_layoutpath );
    void Store( int p_parent_id, int p_id );
    void SetText( int p_id, const dsstring& p_text );
    
    void SubscribePushButtonEventClicked( int p_id );
    void RegisterPushButtonEventClickedHandler( DrawSpace::Core::BaseCallback<void, dsstring>* p_handler );

    // temporaire, pour tests divers :-D
    void InitTest( void );
};

#endif
