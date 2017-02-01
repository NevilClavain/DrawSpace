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

#include <CEGUI\CEGUI.h>
#include "drawspace_commons.h"
#include "callback.h"

#ifndef _CEGUIWRAPPER_H_
#define _CEGUIWRAPPER_H_

class CEGUIWrapper
{
protected:

    typedef DrawSpace::Core::BaseCallback2<void, const dsstring&, const dsstring&>  GUIWidgetPushButtonClickedHandler;

    bool                                                                        m_ready;

    /////////////////////////////////////////////////////////////////////////////////

    typedef std::map<dsstring, CEGUI::Window*>                                  WidgetsTable;

    std::map<CEGUI::Window*, WidgetsTable>                                      m_layoutsTable;
    std::map<dsstring, CEGUI::Window*>                                          m_layoutNamesTable;

    dsstring                                                                    m_currentLayoutName;
    CEGUI::Window*                                                              m_currentLayout;

    std::vector<CEGUI::Editbox*>                                                m_editBoxes;
    std::map<dsstring, CEGUI::Listbox*>                                         m_listBoxes;


    /////////////////////////////////////////////////////////////////////////////////

    //DrawSpace::Core::BaseCallback2<void, const dsstring&, const dsstring &>*    m_pushbuttoneventclicked_handler;
    std::vector<GUIWidgetPushButtonClickedHandler*>                             m_pushbuttoneventclicked_handlers_list;


    CEGUI::MouseCursor*                                 m_mouse_cursor;

    bool            on_PushButton_EventClicked(const CEGUI::EventArgs& p_evt );

    CEGUI::Editbox* find_focused_editbox( void );

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

    void LoadLayout( const dsstring& p_layout_path );

    void LoadScheme( const dsstring& p_scheme_path );

    void SetLayout( const dsstring& p_layoutpath );

    void Store( const dsstring& p_layoutName, const dsstring& p_parentName, int p_id );
    void SubscribePushButtonEventClicked( const dsstring& p_layoutName, const dsstring& p_widgetName );
    void SetText( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text );
    void GetText( const dsstring& p_layoutName, const dsstring& p_widgetName, dsstring& p_outtext );  
    
    void RegisterPushButtonEventClickedHandler( DrawSpace::Core::BaseCallback2<void, const dsstring&, const dsstring&>* p_handler );

    void SetMouseCursorImage( const dsstring& p_image );
    void ShowMouseCursor( bool p_show );


    // temporaire, pour tests divers :-D
    void InitTest( void );
};

#endif
