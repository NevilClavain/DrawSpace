/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "BasicSceneObjectPropertiesDialog.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

BasicSceneObjectPropertiesDialog::BasicSceneObjectPropertiesDialog( wxWindow* parent, const wxString& title )
: ObjectPropertiesDialog( parent, wxID_ANY, title, wxDefaultPosition, wxSize( 381,318 ) ),
m_applybutton_handler( NULL ),
m_specificbutton0_handler( NULL )
{
    m_apply_button->Show( false );
    m_specific_button_0->Show( false );
}

wxPropertyGrid* BasicSceneObjectPropertiesDialog::GetPropertyGrid( void )
{
    return m_propertyGrid;
}

void BasicSceneObjectPropertiesDialog::OnCloseButtonClicked( wxCommandEvent& event )
{
    Close();
}

void BasicSceneObjectPropertiesDialog::OnApplyButtonClicked( wxCommandEvent& event )
{
    if( m_applybutton_handler )
    {
        (*m_applybutton_handler)( this );
    }
}

void BasicSceneObjectPropertiesDialog::OnSpecificButton0Clicked( wxCommandEvent& event )
{
    if( m_specificbutton0_handler )
    {
        (*m_specificbutton0_handler)( this );
    }
}

void BasicSceneObjectPropertiesDialog::EnableApplyButton( void )
{
    m_apply_button->Show( true );
}

void BasicSceneObjectPropertiesDialog::EnableSpecificButton0( const dsstring& p_label )
{
    m_specific_button_0->Show( true );

    m_specific_button_0->SetLabel( p_label );
}

void BasicSceneObjectPropertiesDialog::RegisterApplyButtonHandler( BaseCallback<void, BasicSceneObjectPropertiesDialog*>* p_handler )
{
    m_applybutton_handler = p_handler;
}

void BasicSceneObjectPropertiesDialog::RegisterSpecificButton0Handler( DrawSpace::Core::BaseCallback<void, BasicSceneObjectPropertiesDialog*>* p_handler )
{
    m_specificbutton0_handler = p_handler;
}

void BasicSceneObjectPropertiesDialog::SetData( const dsstring& p_id, void* p_data )
{
    m_datas[p_id] = p_data;
}

void* BasicSceneObjectPropertiesDialog::GetData( const dsstring& p_id )
{
    if( m_datas.count( p_id ) > 0 )
    {
        return m_datas[p_id];
    }
    else
    {
        return NULL;
    }
}
