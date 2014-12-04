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

#include "BasicSceneMainFrame.h"
#include "drawspace.h"
#include "adapters.h"

#include "BasicSceneObjectPropertiesDialog.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

BasicSceneMainFrame::BasicSceneMainFrame( wxWindow* parent ) : MainFrame( parent, wxID_ANY, "DFront Basic Scene", wxDefaultPosition, wxSize( 1264,600 ), wxCAPTION | wxCLOSE_BOX ),
m_glready( false ),
m_scenegraphlistctrl_currentindex( -1 ),
m_mvtslistctrl_currentindex( -1 ),
m_cameraslistctrl_currentindex( -1 ),
m_mousekeyb_output( NULL )
{
    m_transftype_button->Enable( false );
    m_transfoedit_button->Enable( false );
    m_control_button->Enable( false );
    m_cameraedit_button->Enable( false );

    m_timercb = new TimerCallback( this, &BasicSceneMainFrame::on_timer );

    m_timer.AddTimer( "timer", 100, m_timercb );
    m_timer.SetTimerState( "timer", true );
}

wxNotebook* BasicSceneMainFrame::GetNoteBook( void )
{
    return m_notebook2;
}

void BasicSceneMainFrame::compute_regs( void )
{
    for( std::map<dsstring, RegisterEntry>::iterator it = m_registers.begin(); it != m_registers.end(); ++it )
    {
        RegisterEntry* register_entry = &(it->second);

        if( REGISTER_CONSTANT == register_entry->mode )
        {
            register_entry->current_value = register_entry->const_value;
        }
        else if( REGISTER_VARIABLE == register_entry->mode )
        {

        }
    }
}

void BasicSceneMainFrame::on_timer( const dsstring& p_timername )
{
    wxWidgetAdapter::GetInstance()->AdaptRegistersLastValues( &m_registers, m_registers_listCtrl );
}

void BasicSceneMainFrame::OnClose( wxCloseEvent& event )
{
    Destroy();
}

void BasicSceneMainFrame::OnIdle( wxIdleEvent& event )
{
    if( m_glready )
    {
        compute_regs();
        
        DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

        // transform all scenegraph's nodes

        for( std::map<dsstring, MetadataScenegraphEntry>::iterator it = m_metada_scenegraph.begin(); it != m_metada_scenegraph.end(); ++it )
        {
            MetadataScenegraphEntry entry = it->second;

            switch( entry.transformation_source_type )
            {
                case TRANSFORMATIONSOURCE_MATRIXSTACK:
                    {
                        entry.matrix_stack.BuildResult();
                        Matrix mat;
                        entry.matrix_stack.GetResult( &mat );
                        entry.node->SetLocalTransform( mat );
                    }
                    break;

                case TRANSFORMATIONSOURCE_MOVEMENT:

                    break;

                case TRANSFORMATIONSOURCE_BODY:

                    break;
            }
        }
                
        m_scenegraph.ComputeTransformations( m_timer );

        for( size_t i = 0; i < m_ordered_configs.size(); i++ )
        {
            Pass* pass = dynamic_cast<Pass*>( m_ordered_configs[i] );
            if( pass )
            {
                pass->GetRenderingQueue()->Draw();
            }
        }

        renderer->DrawText( 255, 0, 0, 10, 20, "%d fps", m_timer.GetFPS() );

        renderer->FlipScreen();

        m_timer.Update();
        if( m_timer.IsReady() )
        {

        }

        // wxWidget framework specific !
        event.RequestMore( true );
    }
}

void BasicSceneMainFrame::SetGLReady( void )
{
    m_glready = true;
}

void BasicSceneMainFrame::Update( void )
{
    wxWidgetAdapter::GetInstance()->AdaptAssetsList( m_assets_listCtrl );
    wxWidgetAdapter::GetInstance()->AdaptConfigsList( m_configs_listCtrl );
    wxWidgetAdapter::GetInstance()->AdaptPassesList( m_passes_listCtrl );
    wxWidgetAdapter::GetInstance()->AdaptMvtsList( &m_movements, m_mvts_listCtrl );
    wxWidgetAdapter::GetInstance()->AdaptCamerasList( &m_scenegraph, m_cameras_listCtrl );
    wxWidgetAdapter::GetInstance()->AdaptScenegraphList( &m_scenegraph, m_scenegraph_listCtrl );
    wxWidgetAdapter::GetInstance()->AdaptCameraListComboBox( &m_scenegraph, m_cameraslist_comboBox );
    wxWidgetAdapter::GetInstance()->AdaptRegistersList( &m_registers, m_registers_listCtrl );


    ConfigsBase::GetInstance()->GetOrderedConfigsInstancesList( m_ordered_configs );
    for( size_t i = 0; i < m_ordered_configs.size(); i++ )
    {
        Pass* pass = dynamic_cast<Pass*>( m_ordered_configs[i] );
        if( pass )
        {
            m_scenegraph.RegisterPass( pass );
            pass->GetRenderingQueue()->UpdateOutputQueue();
        }
    }
}

void BasicSceneMainFrame::OnAssetsListItemActivated( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();

    Asset* asset = (Asset*)m_assets_listCtrl->GetItemData( sel_index );

    Texture* texture = dynamic_cast<Texture*>( asset );
    if( texture )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Textures properties" );
        wxWidgetAdapter::GetInstance()->AdaptTextureProps( texture, dialog->GetPropertyGrid() );
        dialog->Show();
    }

    Shader* shader = dynamic_cast<Shader*>( asset );
    if( shader )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Shader properties" );
        wxWidgetAdapter::GetInstance()->AdaptShaderProps( shader, dialog->GetPropertyGrid() );
        dialog->Show();
    }

    Font* font = dynamic_cast<Font*>( asset );
    if( font )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Font properties" );
        wxWidgetAdapter::GetInstance()->AdaptFontProps( font, dialog->GetPropertyGrid() );
        dialog->Show();
    }

    Meshe* meshe = dynamic_cast<Meshe*>( asset );
    if( meshe )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Meshe properties" );
        wxWidgetAdapter::GetInstance()->AdaptMesheProps( meshe, dialog->GetPropertyGrid() );
        dialog->Show();
    }
}

void BasicSceneMainFrame::OnConfigsListItemActivated( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();

    Configurable* config = (Configurable*)m_configs_listCtrl->GetItemData( sel_index );

    Fx* fx = dynamic_cast<Fx*>( config );
    if( fx )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Fx properties" );
        wxWidgetAdapter::GetInstance()->AdaptFxProps( fx, dialog->GetPropertyGrid() );
        dialog->Show();
    }

    IntermediatePass* ipass = dynamic_cast<IntermediatePass*>( config );
    if( ipass )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Intermediate Pass properties" );
        wxWidgetAdapter::GetInstance()->AdaptPassProps( true, ipass, dialog->GetPropertyGrid() );
        dialog->Show();
    }

    FinalPass* fpass = dynamic_cast<FinalPass*>( config );
    if( fpass )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Final Pass properties" );
        wxWidgetAdapter::GetInstance()->AdaptPassProps( false, fpass, dialog->GetPropertyGrid() );
        dialog->Show();

    }
}

void BasicSceneMainFrame::OnPassesListItemActivated( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();

    Configurable* config = (Configurable*)m_passes_listCtrl->GetItemData( sel_index );

    IntermediatePass* ipass = dynamic_cast<IntermediatePass*>( config );
    if( ipass )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Intermediate Pass properties" );
        wxWidgetAdapter::GetInstance()->AdaptPassProps( true, ipass, dialog->GetPropertyGrid() );
        dialog->Show();
    }

    FinalPass* fpass = dynamic_cast<FinalPass*>( config );
    if( fpass )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Final Pass properties" );
        wxWidgetAdapter::GetInstance()->AdaptPassProps( false, fpass, dialog->GetPropertyGrid() );
        dialog->Show();
    }
}

void BasicSceneMainFrame::OnPassesListItemSelected( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();
    Configurable* config = (Configurable*)m_passes_listCtrl->GetItemData( sel_index );

    wxWidgetAdapter::GetInstance()->AdaptPassesShaderParamsList( static_cast<Pass*>( config ), m_shadersparams_listCtrl );
}

void BasicSceneMainFrame::OnShadersListItemActivated( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();
    Pass* pass = (Pass*)m_shadersparams_listCtrl->GetItemData( sel_index );

    wxString shader_name = m_shadersparams_listCtrl->GetItemText( sel_index );

    wxCharBuffer buffer = shader_name.ToAscii();

    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Shader parameters modification" );

    dialog->SetData( "ctrl", m_shadersparams_listCtrl );
    wxWidgetAdapter::GetInstance()->AdaptPassShaderValuesPropsModification( pass, buffer.data(), dialog );
    dialog->EnableApplyButton();
    dialog->Show();
}

void BasicSceneMainFrame::OnCreateMvtButtonClicked( wxCommandEvent& p_event )
{
    int index = m_mvttype_comboBox->GetSelection();

    switch( index )
    {
        case 0:
            // Linear
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Linear movement creation" );

                dialog->SetData( "mvts_map", &m_movements );
                dialog->SetData( "ctrl", m_mvts_listCtrl );

                wxWidgetAdapter::GetInstance()->AdaptLinearMvtCreationProps( dialog );
                dialog->EnableApplyButton();
                dialog->Show();
            }
            break;

        case 1:
            // Circular
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Circular movement creation" );

                dialog->SetData( "mvts_map", &m_movements );
                dialog->SetData( "ctrl", m_mvts_listCtrl );

                wxWidgetAdapter::GetInstance()->AdaptCircularMvtCreationProps( dialog );
                dialog->EnableApplyButton();
                dialog->Show();
            }
            break;


        case 2:
            // FPS
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "FPS movement creation" );

                dialog->SetData( "mvts_map", &m_movements );
                dialog->SetData( "ctrl", m_mvts_listCtrl );

                wxWidgetAdapter::GetInstance()->AdaptFPSMvtCreationProps( dialog );
                dialog->EnableApplyButton();
                dialog->Show();
            }
            break;

        case 3:
            // Free
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Free movement creation" );

                dialog->SetData( "mvts_map", &m_movements );
                dialog->SetData( "ctrl", m_mvts_listCtrl );

                wxWidgetAdapter::GetInstance()->AdaptFreeMvtCreationProps( dialog );
                dialog->EnableApplyButton();
                dialog->Show();
            }
            break;

        case 4:
            // Head
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Head movement creation" );

                dialog->SetData( "mvts_map", &m_movements );
                dialog->SetData( "ctrl", m_mvts_listCtrl );

                wxWidgetAdapter::GetInstance()->AdaptHeadMvtCreationProps( dialog );
                dialog->EnableApplyButton();
                dialog->Show();
            }
            break;

        case 5:
            // Spectator
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Spectator movement creation" );

                dialog->SetData( "mvts_map", &m_movements );
                dialog->SetData( "ctrl", m_mvts_listCtrl );

                wxWidgetAdapter::GetInstance()->AdaptSpectatorMvtCreationProps( dialog );
                dialog->EnableApplyButton();
                dialog->Show();
            }
            break;

        case 6:
            // LongLat
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Longlat movement creation" );

                dialog->SetData( "mvts_map", &m_movements );
                dialog->SetData( "ctrl", m_mvts_listCtrl );

                wxWidgetAdapter::GetInstance()->AdaptLongLatMvtCreationProps( dialog );
                dialog->EnableApplyButton();
                dialog->Show();
            }

            break;
    }
}

void BasicSceneMainFrame::OnMvtsListItemActivated( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();

    wxString mvt_name = m_mvts_listCtrl->GetItemText( sel_index );
    wxCharBuffer buffer = mvt_name.ToAscii();

    dsstring mvt_name2 = buffer.data();


    Movement* mvt = (Movement*)m_mvts_listCtrl->GetItemData( sel_index );

    LinearMovement* linearmvt = dynamic_cast<LinearMovement*>( mvt );
    if( linearmvt )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Linear movement properties" );
        wxWidgetAdapter::GetInstance()->AdaptLinearMvtProps( mvt_name2, linearmvt, dialog );
        dialog->Show();
    }

    CircularMovement* circularmvt = dynamic_cast<CircularMovement*>( mvt );
    if( circularmvt )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Circular movement properties" );
        wxWidgetAdapter::GetInstance()->AdaptCircularMvtProps( mvt_name2, circularmvt, dialog );
        dialog->Show();
    }

    FPSMovement* fpsmvt = dynamic_cast<FPSMovement*>( mvt );
    if( fpsmvt )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "FPS movement properties" );
        wxWidgetAdapter::GetInstance()->AdaptFpsMvtProps( mvt_name2, fpsmvt, dialog );
        dialog->Show();
    }

    FreeMovement* freemvt = dynamic_cast<FreeMovement*>( mvt );
    if( freemvt )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Free movement properties" );
        wxWidgetAdapter::GetInstance()->AdaptFreeMvtProps( mvt_name2, freemvt, dialog );
        dialog->Show();
    }

    HeadMovement* headmvt = dynamic_cast<HeadMovement*>( mvt );
    if( headmvt )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Head movement properties" );
        wxWidgetAdapter::GetInstance()->AdaptHeadMvtProps( mvt_name2, headmvt, dialog );
        dialog->Show();
    }
    
    SpectatorMovement* spectatormvt = dynamic_cast<SpectatorMovement*>( mvt );
    if( spectatormvt )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Spectator movement properties" );
        wxWidgetAdapter::GetInstance()->AdaptSpectatorMvtProps( mvt_name2, spectatormvt, dialog );
        dialog->Show();
    }

    LongLatMovement* longlatmvt = dynamic_cast<LongLatMovement*>( mvt );
    if( longlatmvt )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Longlat movement properties" );
        wxWidgetAdapter::GetInstance()->AdaptLongLatMvtProps( mvt_name2, longlatmvt, dialog );
        dialog->Show();
    }

}

void BasicSceneMainFrame::OnCreateCameraButtonClicked( wxCommandEvent& p_event )
{
    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Camera creation" );

    wxWidgetAdapter::GetInstance()->AdaptCameraCreationProps( &m_movements, dialog );

    dialog->SetData( "mvts_map", &m_movements );
    dialog->SetData( "scenegraph", &m_scenegraph );
    dialog->SetData( "cameraslistctrl", m_cameras_listCtrl );
    dialog->SetData( "scenegraphctrl", m_scenegraph_listCtrl );
    dialog->SetData( "cameraslistcombobox", m_cameraslist_comboBox );
    dialog->EnableApplyButton();
    dialog->Show();

}

void BasicSceneMainFrame::OnCamerasListItemActivated( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();
    CameraPoint* camera = (CameraPoint*)m_cameras_listCtrl->GetItemData( sel_index );

    /*
    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Camera properties modifications" );
    wxWidgetAdapter::GetInstance()->AdaptCameraPropsModification( camera, dialog );

    dialog->SetData( "camera", camera );
    dialog->EnableApplyButton();
    dialog->Show();
    */

    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Camera properties" );
    wxWidgetAdapter::GetInstance()->AdaptCameraProps( camera, dialog );
    dialog->Show();

}

void BasicSceneMainFrame::OnCamerasListDeleteAllItems( wxListEvent& p_event )
{
    m_cameraedit_button->Enable( false );
}

void BasicSceneMainFrame::OnCamerasListItemSelected( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();
    m_cameraslistctrl_currentindex = sel_index;
    m_cameraedit_button->Enable( true );
}

void BasicSceneMainFrame::OnScenegraphItemActivated( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();
    
    TransformNode* tnode = (TransformNode*)m_scenegraph_listCtrl->GetItemData( sel_index );

    CameraPoint* camera = dynamic_cast<CameraPoint*>( tnode );
    if( camera )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Camera properties" );
        wxWidgetAdapter::GetInstance()->AdaptCameraProps( camera, dialog );
        dialog->Show();
    }
}

void BasicSceneMainFrame::OnScenegraphListItemSelected( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();
    m_scenegraphlistctrl_currentindex = sel_index;

    TransformNode* transform_node = (TransformNode*)m_scenegraph_listCtrl->GetItemData( sel_index );

    CameraPoint* camera_point = dynamic_cast<CameraPoint*>( transform_node );
    if( camera_point )
    {
        m_transftype_button->Enable( false );
        m_transfoedit_button->Enable( false );
    }

    Spacebox* spacebox = dynamic_cast<Spacebox*>( transform_node );
    if( spacebox )
    {
        m_transftype_button->Enable( true );
        m_transfoedit_button->Enable( true );
    }
}

void BasicSceneMainFrame::OnSetCameraButtonClicked( wxCommandEvent& p_event )
{
    int index = m_cameraslist_comboBox->GetSelection();

    CameraPoint* camera = (CameraPoint*)m_cameraslist_comboBox->GetClientData( index );

    if( NULL == camera )
    {
        m_scenegraph.SetCurrentCamera( "" );
    }
    else
    {
        dsstring camera_name;
        camera->GetSceneName( camera_name );
        m_scenegraph.SetCurrentCamera( camera_name );
    }
}

void BasicSceneMainFrame::OnCreateDrawableButtonClicked( wxCommandEvent& p_event )
{
    int drawable_type = m_drawabletypes_comboBox->GetSelection();

    switch( drawable_type )
    {
        case 0:
            BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Spacebox creation" );
            wxWidgetAdapter::GetInstance()->AdaptSpaceBoxCreationProps( dialog );
            dialog->EnableApplyButton();
            dialog->EnableSpecificButton0( "Add pass slot" );
            dialog->SetData( "scenegraph", &m_scenegraph );
            dialog->SetData( "metadata_scenegraph", &m_metada_scenegraph );
            dialog->SetData( "scenegraphctrl", m_scenegraph_listCtrl );
            dialog->SetData( "configs", &m_ordered_configs );
            dialog->Show();
            break;
    }
}

void BasicSceneMainFrame::OnTransfTypeButtonClicked( wxCommandEvent& p_event )
{
    long sel_index = m_scenegraphlistctrl_currentindex;

    TransformNode* transform_node = (TransformNode*)m_scenegraph_listCtrl->GetItemData( sel_index );

    CameraPoint* camera_point = dynamic_cast<CameraPoint*>( transform_node );
    if( camera_point )
    {
    }
    Spacebox* spacebox = dynamic_cast<Spacebox*>( transform_node );
    if( spacebox )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Spacebox transformation source selection" );

        dsstring scene_name;
        spacebox->GetSceneName( scene_name );
        
        dialog->SetData( "metadata_scenegraph_entry", &m_metada_scenegraph[scene_name] );
        dialog->EnableApplyButton();
        wxWidgetAdapter::GetInstance()->AdaptTransfoSourceModification( dialog );
        dialog->Show();
    }
}

void BasicSceneMainFrame::OnTransfoEditButtonClicked( wxCommandEvent& p_event )
{
    long sel_index = m_scenegraphlistctrl_currentindex;

    TransformNode* transform_node = (TransformNode*)m_scenegraph_listCtrl->GetItemData( sel_index );

    CameraPoint* camera_point = dynamic_cast<CameraPoint*>( transform_node );
    if( camera_point )
    {
    }
    Spacebox* spacebox = dynamic_cast<Spacebox*>( transform_node );
    if( spacebox )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Spacebox transformation source edition" );

        dsstring scene_name;
        spacebox->GetSceneName( scene_name );

        dialog->SetData( "metadata_scenegraph_entry", &m_metada_scenegraph[scene_name] );
        dialog->EnableApplyButton();
        dialog->EnableSpecificButton0( "Add matrix" );
        dialog->EnableSpecificButton1( "Clear all" );
        wxWidgetAdapter::GetInstance()->AdaptMatrixStackEdition( dialog );
        dialog->Show();
    }
}

void BasicSceneMainFrame::OnScenegraphListItemDeselected( wxListEvent& p_event )
{
}

void BasicSceneMainFrame::OnScenegraphListDeleteItem( wxListEvent& p_event )
{
}

void BasicSceneMainFrame::OnScenegraphListDeleteAllItems( wxListEvent& p_event )
{
    m_transftype_button->Enable( false );
    m_transfoedit_button->Enable( false );
}

void BasicSceneMainFrame::OnControlButtonClicked( wxCommandEvent& p_event )
{
    wxString mvt_name = m_mvts_listCtrl->GetItemText( m_mvtslistctrl_currentindex );
    wxCharBuffer buffer = mvt_name.ToAscii();
    dsstring mvt_name2 = buffer.data();

    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Movement control edition" );

    dialog->SetData( "movement_entry", &m_movements[mvt_name2] );
    dialog->EnableApplyButton();

    wxWidgetAdapter::GetInstance()->AdaptMovementEntryControlProps( mvt_name2, dialog );
    dialog->Show();
}

void BasicSceneMainFrame::OnMouseKeyboardOutputButtonClicked( wxCommandEvent& p_event )
{

}

void BasicSceneMainFrame::OnMvtsListDeleteAllItems( wxListEvent& p_event )
{
    m_control_button->Enable( false );
}

void BasicSceneMainFrame::OnMvtsListItemSelected( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();
    m_mvtslistctrl_currentindex = sel_index;

    wxString mvt_name = m_mvts_listCtrl->GetItemText( sel_index );
    wxCharBuffer buffer = mvt_name.ToAscii();
    dsstring mvt_name2 = buffer.data();

    MovementEntry movement_entry = m_movements[mvt_name2];

    LinearMovement* linearmvt = dynamic_cast<LinearMovement*>( movement_entry.movement );
    if( linearmvt )
    {
        m_control_button->Enable( true );
    }

    CircularMovement* circularmvt = dynamic_cast<CircularMovement*>( movement_entry.movement );
    if( circularmvt )
    {
        m_control_button->Enable( true );
    }

    FPSMovement* fpsmvt = dynamic_cast<FPSMovement*>( movement_entry.movement );
    if( fpsmvt )
    {
        m_control_button->Enable( true );
    }

    FreeMovement* freemvt = dynamic_cast<FreeMovement*>( movement_entry.movement );
    if( freemvt )
    {
        m_control_button->Enable( true );
    }

    HeadMovement* headmvt = dynamic_cast<HeadMovement*>( movement_entry.movement );
    if( headmvt )
    {
        m_control_button->Enable( false );
    }

    SpectatorMovement* spectatormvt = dynamic_cast<SpectatorMovement*>( movement_entry.movement );
    if( spectatormvt )
    {
        m_control_button->Enable( false );
    }

    LongLatMovement* longlatmvt = dynamic_cast<LongLatMovement*>( movement_entry.movement );
    if( longlatmvt )
    {
        m_control_button->Enable( true );
    }
}

void BasicSceneMainFrame::OnCreateRegButtonClicked( wxCommandEvent& p_event )
{
    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Register creation" );

    dialog->SetData( "registers_map", &m_registers );
    dialog->SetData( "ctrl", m_registers_listCtrl );

    dialog->EnableApplyButton();

    wxWidgetAdapter::GetInstance()->AdaptRegisterCreationProps( dialog );
    dialog->Show();
    
}

void BasicSceneMainFrame::OnCameraEditButtonClicked( wxCommandEvent& p_event )
{
    CameraPoint* camera = (CameraPoint*)m_cameras_listCtrl->GetItemData( m_cameraslistctrl_currentindex );
    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Camera properties modifications" );
    wxWidgetAdapter::GetInstance()->AdaptCameraPropsModification( camera, dialog );

    dialog->SetData( "camera", camera );
    dialog->EnableApplyButton();
    dialog->Show();
}