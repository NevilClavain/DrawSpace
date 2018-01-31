/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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


#include <CEGUI\CEGUI.h>
#include "drawspace_commons.h"
#include "vector.h"
#include "callback.h"
#include "jsonparser.h"

#ifndef _CEGUIWRAPPER_H_
#define _CEGUIWRAPPER_H_

class CEGUIWrapper
{
protected:

    typedef DrawSpace::Core::BaseCallback2<void, const dsstring&, const dsstring&>          GUIWidgetPushButtonClickedHandler;
    typedef DrawSpace::Core::BaseCallback3<void, const dsstring&, const dsstring&, bool>    GUIWidgetCheckboxStateChangedHandler;



    bool                                                                        m_ready;

    /////////////////////////////////////////////////////////////////////////////////

    typedef std::map<dsstring, CEGUI::Window*>                                  WidgetsTable;

    std::map<CEGUI::Window*, WidgetsTable>                                      m_layoutsTable;
    std::map<dsstring, CEGUI::Window*>                                          m_layoutNamesTable;

    dsstring                                                                    m_currentLayoutName;
    CEGUI::Window*                                                              m_currentLayout;

    std::vector<CEGUI::Editbox*>                                                m_editBoxes;

    std::map<dsstring, CEGUI::Window*>                                          m_spritesTable;


    /////////////////////////////////////////////////////////////////////////////////

    std::vector<GUIWidgetPushButtonClickedHandler*>                             m_pushbuttoneventclicked_handlers_list;
    std::vector<GUIWidgetCheckboxStateChangedHandler*>                          m_checkboxeventstatechanged_handlers_list;

    CEGUI::MouseCursor*                                                         m_mouse_cursor;

    bool            on_PushButton_EventClicked(const CEGUI::EventArgs& p_evt );

    bool            on_ToggleButton_EventSelectStateChanged(const CEGUI::EventArgs& p_evt );

    void            recurs_register_widgets( DrawSpace::Utils::JSONParser& p_parser, int& p_token_index, const dsstring& p_layout_path, const dsstring& p_parent );

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

    void LoadLayout( const dsstring& p_layout_path, const dsstring& p_widgets_path );

    void LoadScheme( const dsstring& p_scheme_path );

    void SetLayout( const dsstring& p_layoutpath );

    void Store( const dsstring& p_layoutName, const dsstring& p_parentName, const dsstring& p_childName );

    void SetText( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text );
    void GetText( const dsstring& p_layoutName, const dsstring& p_widgetName, dsstring& p_outtext );

    void SetVisibleState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state );
    bool IsVisible( const dsstring& p_layoutName, const dsstring& p_widgetName );

    void AddListboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage );
    void ClearListbox( const dsstring& p_layoutName, const dsstring& p_widgetName );
    bool GetListboxFirstSelectedItemIndex( const dsstring& p_layoutName, const dsstring& p_widgetName, int& p_index, dsstring& p_text );

    void AddComboboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage );
    void ClearCombobox( const dsstring& p_layoutName, const dsstring& p_widgetName );

    void SetComboBoxItemSelectionState( const dsstring& p_layoutName, const dsstring& p_widgetName, int p_index, bool p_state );
    int  GetComboBoxSelectionIndex( const dsstring& p_layoutName, const dsstring& p_widgetName );

    bool IsCheckBoxChecked( const dsstring& p_layoutName, const dsstring& p_widgetName );
    void SetCheckboxState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state );

    
    void RegisterPushButtonEventClickedHandler( DrawSpace::Core::BaseCallback2<void, const dsstring&, const dsstring&>* p_handler );
    void RegisterCheckboxEventStateChangedHandler( DrawSpace::Core::BaseCallback3<void, const dsstring&, const dsstring&, bool>* p_handler );

    void SetMouseCursorImage( const dsstring& p_image );
    void ShowMouseCursor( bool p_show );

    void CreateSprite( const dsstring& p_scheme_object, const dsstring& p_spriteName );
    void SetSpritePosition( const dsstring& p_spriteName, dsreal p_xpos, dsreal p_ypos );
    void SetSpriteImage( const dsstring& p_spriteName, const dsstring& p_image );
    void SetSpriteScale( const dsstring& p_spriteName, dsreal p_scale );
    void SetSpriteRotation( const dsstring& p_spriteName, const DrawSpace::Utils::Vector& p_axis, dsreal p_deg_angle );
    
    // temporaire, pour tests divers :-D
    void InitTest( void );
};

#endif
