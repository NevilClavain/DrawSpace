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

BasicSceneMainFrame::BasicSceneMainFrame( wxWindow* parent ) : MainFrame( parent ),
m_glready( false )
{

}

void BasicSceneMainFrame::OnClose( wxCloseEvent& event )
{
    Destroy();
}

void BasicSceneMainFrame::OnIdle( wxIdleEvent& event )
{
    if( m_glready )
    {

        DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
        
        m_scenegraph.ComputeTransformations( m_timer );

        /*
        renderer->ClearScreen( 0, 0, 120 );
        renderer->BeginScreen();
        renderer->DrawText( 255, 0, 0, 10, 20, "%d fps", m_timer.GetFPS() );
        renderer->EndScreen();
        */


        for( size_t i = 0; i < m_ordered_configs.size(); i++ )
        {
            Pass* pass = dynamic_cast<Pass*>( m_ordered_configs[i] );
            if( pass )
            {
                pass->GetRenderingQueue()->Draw();
            }
        }


        renderer->BeginScreen();
        renderer->DrawText( 255, 0, 0, 10, 20, "%d fps", m_timer.GetFPS() );
        renderer->EndScreen();

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

    ConfigsBase::GetInstance()->GetOrderedConfigsInstancesList( m_ordered_configs );
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

    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Camera properties modifications" );
    wxWidgetAdapter::GetInstance()->AdaptCameraPropsModification( camera, dialog );

    dialog->SetData( "camera", camera );
    dialog->EnableApplyButton();
    dialog->Show();
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
    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Spacebox creation" );
    wxWidgetAdapter::GetInstance()->AdaptSpaceBoxCreationProps( dialog );
    dialog->EnableApplyButton();
    dialog->EnableSpecificButton0( "Add pass slot" );
    dialog->Show();
}