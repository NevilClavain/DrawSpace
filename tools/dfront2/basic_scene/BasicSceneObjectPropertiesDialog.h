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

#ifndef __BasicSceneObjectPropertiesDialog__
#define __BasicSceneObjectPropertiesDialog__


#include "panel.h"
#include "drawspace.h"

class BasicSceneObjectPropertiesDialog : public ObjectPropertiesDialog
{
protected:

    DrawSpace::Core::BaseCallback<void, BasicSceneObjectPropertiesDialog*>*     m_applybutton_handler;
    DrawSpace::Core::BaseCallback<void, BasicSceneObjectPropertiesDialog*>*     m_specificbutton0_handler;
    DrawSpace::Core::BaseCallback<void, BasicSceneObjectPropertiesDialog*>*     m_specificbutton1_handler;
    DrawSpace::Core::BaseCallback<void, BasicSceneObjectPropertiesDialog*>*     m_specificbutton2_handler;
    std::map<dsstring, void*>                                                   m_datas;

    wxString                                                                    m_title;

    long                                                                        m_specific0_counter;
    long                                                                        m_specific1_counter;
    long                                                                        m_specific2_counter;

    wxTreeItemId                                                                m_treeitem;

    virtual void OnCloseButtonClicked( wxCommandEvent& event );
    virtual void OnApplyButtonClicked( wxCommandEvent& event );
    virtual void OnSpecificButton0Clicked( wxCommandEvent& event );
    virtual void OnSpecificButton1Clicked( wxCommandEvent& event );
    virtual void OnSpecificButton2Clicked( wxCommandEvent& event );

public:

    BasicSceneObjectPropertiesDialog( wxWindow* parent, const wxString& title, wxTreeItemId p_treeitem );
    wxPropertyGrid* GetPropertyGrid( void );

    void EnableApplyButton( void );
    void EnableSpecificButton0( const dsstring& p_label );
    void EnableSpecificButton1( const dsstring& p_label );
    void EnableSpecificButton2( const dsstring& p_label );
    void RegisterApplyButtonHandler( DrawSpace::Core::BaseCallback<void, BasicSceneObjectPropertiesDialog*>* p_handler );
    void RegisterSpecificButton0Handler( DrawSpace::Core::BaseCallback<void, BasicSceneObjectPropertiesDialog*>* p_handler );
    void RegisterSpecificButton1Handler( DrawSpace::Core::BaseCallback<void, BasicSceneObjectPropertiesDialog*>* p_handler );
    void RegisterSpecificButton2Handler( DrawSpace::Core::BaseCallback<void, BasicSceneObjectPropertiesDialog*>* p_handler );

    void SetData( const dsstring& p_id, void* p_data );
    void* GetData( const dsstring& p_id );

    wxString GetTitle( void );

    long GetSpecific0Counter( void );
    long GetSpecific1Counter( void );
    long GetSpecific2Counter( void );

    void SetSpecific0Counter( long p_value );
    void SetSpecific1Counter( long p_value );
    void SetSpecific2Counter( long p_value );

    wxTreeItemId GetTreeItem( void );
};

#endif // __BasicSceneObjectPropertiesDialog__
