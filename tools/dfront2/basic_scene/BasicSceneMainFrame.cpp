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
m_regslistctrl_currentindex( -1 ),
m_last_xmouse( 0 ),
m_last_ymouse( 0 ),
m_current_camera( NULL )
{
    m_transftype_button->Enable( false );
    m_transfoedit_button->Enable( false );
    m_control_button->Enable( false );
    m_cameraedit_button->Enable( false );
    m_modreg_button->Enable( false );
    m_resetreg_button->Enable( false );
    m_regon_button->Enable( false );
    m_regoff_button->Enable( false );


    m_timercb = new TimerCallback( this, &BasicSceneMainFrame::on_timer );

    m_timer.AddTimer( "timer", 100, m_timercb );
    m_timer.SetTimerState( "timer", true );





    m_scripting = DrawSpace::Core::SingletonPlugin<Scripting>::GetInstance()->m_interface;

    m_scripting->Initialize();

    m_scripting_error_cb = new ScriptingErrorCallback( this, &BasicSceneMainFrame::on_scripting_error );
    m_scripting->RegisterScriptErrorHandler( m_scripting_error_cb );

    m_scripting_print_cb = new ScriptingPrintCallback( this, &BasicSceneMainFrame::on_scripting_print );
    m_scripting->RegisterScriptGlobalPrintHandler( m_scripting_print_cb );

    m_scripting_dsdisplayframerate_cb = new ScriptingDrawspaceDisplayFramerateCallback( this, &BasicSceneMainFrame::on_scripting_displayframerate );
    m_scripting->RegisterDrawspaceDisplayFramerateHandler( m_scripting_dsdisplayframerate_cb );

}


void BasicSceneMainFrame::on_scripting_error( const dsstring& p_error )
{
    _asm nop
}

void BasicSceneMainFrame::on_scripting_print( const dsstring& p_text )
{
    _asm nop
}

void BasicSceneMainFrame::on_scripting_displayframerate( bool p_display )
{
    _asm nop
}

void BasicSceneMainFrame::ExecStartupScript( const dsstring& p_scriptfilepath )
{
    // TODO
    m_scripting->ExecFile( p_scriptfilepath.c_str() );
}

wxNotebook* BasicSceneMainFrame::GetNoteBook( void )
{
    return m_notebook2;
}

void BasicSceneMainFrame::SetWindowDims( long p_w_width, long p_w_height )
{
    m_w_width = p_w_width;
    m_w_height = p_w_height;
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
        else if( REGISTER_VARIABLE == register_entry->mode && register_entry->state )
        {
            switch( register_entry->variable_mode )
            {
                case REGISTER_VARIABLE_TRANSLATION_SIMPLE:
                    {
                        if( register_entry->current_value > register_entry->variable_range_inf && 
                            register_entry->current_value < register_entry->variable_range_sup )
                        {
                            m_timer.TranslationSpeedInc( &register_entry->current_value, register_entry->variable_speed );
                        }
                    }
                    break;

                case REGISTER_VARIABLE_TRANSLATION_ROUNDTRIP:
                    {
                        if( !register_entry->variable_roundtrip_back )
                        {
                            if( register_entry->current_value <= register_entry->variable_range_sup )
                            {
                                m_timer.TranslationSpeedInc( &register_entry->current_value, register_entry->variable_speed );
                            }
                            else
                            {
                                register_entry->variable_roundtrip_back = true;
                            }
                        }
                        else
                        {
                            if( register_entry->current_value >= register_entry->variable_range_inf )
                            {
                                m_timer.TranslationSpeedDec( &register_entry->current_value, register_entry->variable_speed );
                            }
                            else
                            {
                                register_entry->variable_roundtrip_back = false;
                            }
                        }
                    }
                    break;

                case REGISTER_VARIABLE_ANGULAR_SIMPLE:
                    {
                        if( register_entry->current_value > register_entry->variable_range_inf && 
                            register_entry->current_value < register_entry->variable_range_sup )
                        {
                            m_timer.AngleSpeedInc( &register_entry->current_value, register_entry->variable_speed );
                        }
                    }
                    break;

                case REGISTER_VARIABLE_ANGULAR_ROUNDTRIP:
                    {
                        if( register_entry->discontinuity )
                        {
                            if( !register_entry->variable_roundtrip_back )
                            {
                                if( register_entry->variable_speed > 0.0 )
                                {
                                    m_timer.AngleSpeedInc( &register_entry->current_value, register_entry->variable_speed );
                                }
                                else
                                {
                                    m_timer.AngleSpeedDec( &register_entry->current_value, -register_entry->variable_speed );
                                }

                                if( !( ( 0.0 <= register_entry->current_value && register_entry->current_value <= register_entry->variable_range_sup ) ||
                                      ( register_entry->variable_range_inf <= register_entry->current_value && register_entry->current_value < 360.0 ) ) )
                                {
                                    register_entry->variable_roundtrip_back = true;
                                }
                            }
                            else
                            {

                                if( register_entry->variable_speed > 0.0 )
                                {
                                    m_timer.AngleSpeedDec( &register_entry->current_value, register_entry->variable_speed );
                                }
                                else
                                {
                                    m_timer.AngleSpeedInc( &register_entry->current_value, -register_entry->variable_speed );
                                }

                                if( !( ( 0.0 <= register_entry->current_value && register_entry->current_value <= register_entry->variable_range_sup ) ||
                                      ( register_entry->variable_range_inf <= register_entry->current_value && register_entry->current_value < 360.0 ) ) )
                                {
                                    register_entry->variable_roundtrip_back = false;
                                }
                            }
                        }
                        else
                        {
                            if( !register_entry->variable_roundtrip_back )
                            {
                                m_timer.AngleSpeedInc( &register_entry->current_value, register_entry->variable_speed );

                                if( !( register_entry->variable_range_inf <= register_entry->current_value && register_entry->current_value <= register_entry->variable_range_sup ) )
                                {
                                    register_entry->variable_roundtrip_back = true;
                                }
                            }
                            else
                            {
                                m_timer.AngleSpeedDec( &register_entry->current_value, register_entry->variable_speed );

                                if( !( register_entry->variable_range_inf <= register_entry->current_value && register_entry->current_value <= register_entry->variable_range_sup ) )
                                {
                                    register_entry->variable_roundtrip_back = false;
                                }
                            }
                        }
                    }
                    break;
            }
        }
    }
}

void BasicSceneMainFrame::on_timer( const dsstring& p_timername )
{
    wxWidgetAdapter::GetInstance()->AdaptRegistersLastValue( &m_registers, m_registers_listCtrl );
}

void BasicSceneMainFrame::OnClose( wxCloseEvent& p_event )
{
    DrawSpace::Core::SingletonPlugin<Scripting>::GetInstance()->m_interface->Shutdown();
    Destroy();
}

void BasicSceneMainFrame::compute_scenegraph_transforms( void )
{
    // transform all scenegraph's nodes

    for( std::map<dsstring, MetadataScenegraphEntry>::iterator it = m_metada_scenegraph.begin(); it != m_metada_scenegraph.end(); ++it )
    {
        MetadataScenegraphEntry entry = it->second;

        switch( entry.transformation_source_type )
        {
            case TRANSFORMATIONSOURCE_MATRIXSTACK:
                {
                    /*
                    entry.matrix_stack.BuildResult();
                    Matrix mat;
                    entry.matrix_stack.GetResult( &mat );
                    entry.node->SetLocalTransform( mat );
                    */

                    DrawSpace::Utils::Transformation matrix_stack;

                    for( size_t i = 0; i < entry.matrix_stack_descr.size(); i++ )
                    {
                        Matrix mat;

                        switch( entry.matrix_stack_descr[i].ope )
                        {
                            case TRANSFORMATIONMATRIX_IDENTITY:
                                break;

                            case TRANSFORMATIONMATRIX_SCALE:
                                {
                                    dsreal scale[3];

                                    for( long j = 0; j < 3; j++ )
                                    {
                                        if( "..." == entry.matrix_stack_descr[i].arg.scale_vals_link[j].var_alias )
                                        {
                                            scale[j] = entry.matrix_stack_descr[i].arg.scale_vals_link[j].value;
                                        }
                                        else
                                        {
                                            dsstring var_alias = entry.matrix_stack_descr[i].arg.scale_vals_link[j].var_alias;
                                            // aller chercher curren_value de la variable referencee

                                            RegisterEntry reg_entry = m_registers[var_alias];
                                            scale[j] = reg_entry.current_value;
                                        }
                                    }

                                    mat.Scale( scale[0], scale[1], scale[2] );
                                    matrix_stack.PushMatrix( mat );
                                }
                                break;

                            case TRANSFORMATIONMATRIX_TRANSLATION:
                                {
                                    dsreal trans[3];

                                    for( long j = 0; j < 3; j++ )
                                    {
                                        if( "..." == entry.matrix_stack_descr[i].arg.translation_vals_link[j].var_alias )
                                        {
                                            trans[j] = entry.matrix_stack_descr[i].arg.translation_vals_link[j].value;
                                        }
                                        else
                                        {
                                            dsstring var_alias = entry.matrix_stack_descr[i].arg.translation_vals_link[j].var_alias;
                                            // aller chercher curren_value de la variable referencee

                                            RegisterEntry reg_entry = m_registers[var_alias];
                                            trans[j] = reg_entry.current_value;
                                        }
                                    }

                                    mat.Translation( trans[0], trans[1], trans[2] );
                                    matrix_stack.PushMatrix( mat );
                                }
                                break;

                            case TRANSFORMATIONMATRIX_ROTATION:
                                {
                                    dsreal axis[3];
                                    dsreal angle;

                                    for( long j = 0; j < 3; j++ )
                                    {
                                        if( "..." == entry.matrix_stack_descr[i].arg.rotation_vals_link[j].var_alias )
                                        {
                                            axis[j] = entry.matrix_stack_descr[i].arg.rotation_vals_link[j].value;
                                        }
                                        else
                                        {
                                            dsstring var_alias = entry.matrix_stack_descr[i].arg.rotation_vals_link[j].var_alias;
                                            // aller chercher curren_value de la variable referencee

                                            RegisterEntry reg_entry = m_registers[var_alias];
                                            axis[j] = reg_entry.current_value;
                                        }
                                    }

                                    if( "..." == entry.matrix_stack_descr[i].arg.angle_val_link.var_alias )
                                    {
                                        angle = entry.matrix_stack_descr[i].arg.angle_val_link.value;
                                    }
                                    else
                                    {
                                        dsstring var_alias = entry.matrix_stack_descr[i].arg.angle_val_link.var_alias;
                                        // aller chercher curren_value de la variable referencee

                                        RegisterEntry reg_entry = m_registers[var_alias];
                                        angle = reg_entry.current_value;
                                    }

                                    mat.Rotation( Vector( axis[0], axis[1], axis[2], 1.0 ), Maths::DegToRad( angle ) );
                                    matrix_stack.PushMatrix( mat );
                                }
                                break;
                        }
                    }


                    matrix_stack.BuildResult();
                    Matrix mat_res;
                    matrix_stack.GetResult( &mat_res );
                    entry.node->SetLocalTransform( mat_res );
                }
                break;

            case TRANSFORMATIONSOURCE_MOVEMENT:

                break;

            case TRANSFORMATIONSOURCE_BODY:

                break;
        }
    }

}

void BasicSceneMainFrame::OnKeyDown( wxKeyEvent& p_event )
{
    int index = m_mousekeyboardoutput_comboBox->GetSelection();
    MovementEntry* movement_entry = (MovementEntry*)m_mousekeyboardoutput_comboBox->GetClientData( index );
    if( movement_entry )
    {
        LinearMovement* linear_movement = dynamic_cast<LinearMovement*>( movement_entry->movement );
        if( linear_movement )
        {
            if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->speed_control_source )
            {
                if( 'Q' == p_event.GetKeyCode() )
                {
                    linear_movement->SetSpeed( 3.0 );
                }
                else if( 'A' == p_event.GetKeyCode() )
                {
                    linear_movement->SetSpeed( -3.0 );
                }
            }
        }

        CircularMovement* circular_movement = dynamic_cast<CircularMovement*>( movement_entry->movement );
        if( circular_movement )
        {
            if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->speed_control_source )
            {                
                if( 'Q' == p_event.GetKeyCode() )
                {
                    circular_movement->SetAngularSpeed( 10.0 );
                }
                else if( 'A' == p_event.GetKeyCode() )
                {
                    circular_movement->SetAngularSpeed( -10.0 );
                }
            }
        }

        FPSMovement* fps_movement = dynamic_cast<FPSMovement*>( movement_entry->movement );
        if( fps_movement )
        {
            if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->speed_control_source )
            {
                if( 'Q' == p_event.GetKeyCode() )
                {
                    fps_movement->SetSpeed( 3.0 );
                }
                else if( 'A' == p_event.GetKeyCode() )
                {
                    fps_movement->SetSpeed( -3.0 );
                }
            }
        }

        FreeMovement* free_movement = dynamic_cast<FreeMovement*>( movement_entry->movement );
        if( free_movement )
        {
            if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->speed_control_source )
            {
                if( 'Q' == p_event.GetKeyCode() )
                {
                    fps_movement->SetSpeed( 3.0 );
                }
                else if( 'A' == p_event.GetKeyCode() )
                {
                    fps_movement->SetSpeed( -3.0 );
                }
            }
        }
    }
}
void BasicSceneMainFrame::OnKeyUp( wxKeyEvent& p_event )
{

}

void BasicSceneMainFrame::OnMouseMotion( wxMouseEvent& p_event )
{
    if( !p_event.Dragging() )
    {
        return;
    }

    wxCoord curr_xmouse = p_event.GetX();
    wxCoord curr_ymouse = p_event.GetY();

    if( curr_xmouse < m_w_width / 4 )
    {
        curr_xmouse = 3 * m_w_width / 4;
        WarpPointer( curr_xmouse, curr_ymouse );
        m_last_xmouse = curr_xmouse;
        m_last_ymouse = curr_ymouse;
        return;
    }
    else if( curr_xmouse > 3 * m_w_width / 4 )
    {
        curr_xmouse = m_w_width / 4;
        WarpPointer( curr_xmouse, curr_ymouse );
        m_last_xmouse = curr_xmouse;
        m_last_ymouse = curr_ymouse;
        return;
    }

    if( curr_ymouse < m_w_height / 4 )
    {
        curr_ymouse = 3 * m_w_height / 4;
        WarpPointer( curr_xmouse, curr_ymouse );
        m_last_xmouse = curr_xmouse;
        m_last_ymouse = curr_ymouse;
        return;
    }
    else if( curr_ymouse > 3 * m_w_height / 4 )
    {
        curr_ymouse = m_w_height / 4;
        WarpPointer( curr_xmouse, curr_ymouse );
        m_last_xmouse = curr_xmouse;
        m_last_ymouse = curr_ymouse;
        return;
    }


    int delta_x = curr_xmouse - m_last_xmouse;
    int delta_y = curr_ymouse - m_last_ymouse;
    
    int index = m_mousekeyboardoutput_comboBox->GetSelection();

    MovementEntry* movement_entry = (MovementEntry*)m_mousekeyboardoutput_comboBox->GetClientData( index );
    if( movement_entry )
    {

        LinearMovement* linear_movement = dynamic_cast<LinearMovement*>( movement_entry->movement );
        if( linear_movement )
        {
            if( p_event.LeftIsDown() )
            {
                if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->yaw_control_source )
                {
                    m_timer.AngleSpeedInc( &movement_entry->theta_pos_mouse, -20.0 * delta_x );
                    linear_movement->SetTheta( movement_entry->theta_pos_mouse );
                }

                if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->pitch_control_source )
                {
                    m_timer.AngleSpeedInc( &movement_entry->phi_pos_mouse, -20.0 * delta_y );
                    linear_movement->SetPhi( movement_entry->phi_pos_mouse );
                }
            }
        }

        CircularMovement* circular_movement = dynamic_cast<CircularMovement*>( movement_entry->movement );
        if( circular_movement )
        {
            if( p_event.LeftIsDown() )
            {
                if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->yaw_control_source )
                {
                    m_timer.AngleSpeedInc( &movement_entry->theta_pos_mouse, -20.0 * delta_x );
                    circular_movement->SetTheta( movement_entry->theta_pos_mouse );
                }
                 
                if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->pitch_control_source )
                {
                    m_timer.AngleSpeedInc( &movement_entry->phi_pos_mouse, -20.0 * delta_y );
                    circular_movement->SetPhi( movement_entry->phi_pos_mouse );
                }
            }
        }

        FPSMovement* fps_movement = dynamic_cast<FPSMovement*>( movement_entry->movement );
        if( fps_movement )
        {
            if( p_event.LeftIsDown() )
            {
                if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->yaw_control_source )
                {
                    fps_movement->RotateYaw( -delta_x / 4.0, m_timer );
                }

                if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->pitch_control_source )
                {
                    fps_movement->RotatePitch( -delta_y / 4.0, m_timer );
                }
            }
        }

        FreeMovement* free_movement = dynamic_cast<FreeMovement*>( movement_entry->movement );
        if( free_movement )
        {
            if( p_event.LeftIsDown() )
            {
                if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->yaw_control_source )
                {
                    free_movement->RotateYaw( -delta_x / 4.0, m_timer );
                }

                if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->pitch_control_source )
                {
                    free_movement->RotatePitch( -delta_y / 4.0, m_timer );
                }
            }
            else if( p_event.RightIsDown() )
            {
                if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->roll_control_source )
                {
                    free_movement->RotateRoll( -delta_x / 4.0, m_timer );
                }
            }
        }

        LongLatMovement* longlat_movement = dynamic_cast<LongLatMovement*>( movement_entry->movement );
        if( longlat_movement )
        {
            if( p_event.LeftIsDown() )
            {
                if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->yaw_control_source )
                {
                    m_timer.AngleSpeedInc( &movement_entry->theta_pos_mouse, -20.0 * delta_x );
                    longlat_movement->SetTheta( movement_entry->theta_pos_mouse );
                }

                if( MOVEMENTCONTROLSOURCE_KEYBMOUSE == movement_entry->pitch_control_source )
                {
                    m_timer.AngleSpeedInc( &movement_entry->phi_pos_mouse, -20.0 * delta_y );
                    longlat_movement->SetPhi( movement_entry->phi_pos_mouse );
                }
            }
        }
    }

    m_last_xmouse = curr_xmouse;
    m_last_ymouse = curr_ymouse;
}

void BasicSceneMainFrame::compute_movements( void )
{
    for( std::map<dsstring, MovementEntry>::iterator it = m_movements.begin(); it != m_movements.end(); ++it )
    {
        MovementEntry movement_entry = it->second;

        Movement* movement = movement_entry.movement;

        LinearMovement* linear_movement = dynamic_cast<LinearMovement*>( movement );
        if( linear_movement )
        {
            if( MOVEMENTCONTROLSOURCE_REGISTER == movement_entry.yaw_control_source )
            {
                linear_movement->SetTheta( m_registers[movement_entry.yaw_control_register].current_value );
            }
            if( MOVEMENTCONTROLSOURCE_REGISTER == movement_entry.pitch_control_source )
            {
                linear_movement->SetPhi( m_registers[movement_entry.pitch_control_register].current_value );
            }
            if( MOVEMENTCONTROLSOURCE_REGISTER == movement_entry.speed_control_source )
            {
                linear_movement->SetSpeed( m_registers[movement_entry.speed_control_register].current_value );
            }
        }

        CircularMovement* circular_movement = dynamic_cast<CircularMovement*>( movement );
        if( circular_movement )
        {
            if( MOVEMENTCONTROLSOURCE_REGISTER == movement_entry.yaw_control_source )
            {
                circular_movement->SetTheta( m_registers[movement_entry.yaw_control_register].current_value );
            }

            if( MOVEMENTCONTROLSOURCE_REGISTER == movement_entry.pitch_control_source )
            {
                circular_movement->SetPhi( m_registers[movement_entry.pitch_control_register].current_value );
            }
            if( MOVEMENTCONTROLSOURCE_REGISTER == movement_entry.speed_control_source )
            {
                circular_movement->SetAngularSpeed( m_registers[movement_entry.speed_control_register].current_value );
            }
        }

        LongLatMovement* longlat_movement = dynamic_cast<LongLatMovement*>( movement );
        if( longlat_movement )
        {
            if( MOVEMENTCONTROLSOURCE_REGISTER == movement_entry.yaw_control_source )
            {
                longlat_movement->SetTheta( m_registers[movement_entry.yaw_control_register].current_value );
            }

            if( MOVEMENTCONTROLSOURCE_REGISTER == movement_entry.pitch_control_source )
            {
                longlat_movement->SetPhi( m_registers[movement_entry.pitch_control_register].current_value );
            }
        }
    }
}


void BasicSceneMainFrame::OnIdle( wxIdleEvent& p_event )
{
    if( m_glready )
    {
        int index = m_cameraslist_comboBox->GetSelection();
        CameraPoint* camera = (CameraPoint*)m_cameraslist_comboBox->GetClientData( index );

        if( camera != m_current_camera )
        {
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
            m_current_camera = camera;
        }

        compute_regs();
        
        DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

        compute_movements();
        compute_scenegraph_transforms();
                
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
        
        dsstring camera_name;
        m_scenegraph.GetCurrentCameraName( camera_name );
        if( "" == camera_name )
        {
            camera_name = "...";
        }

        renderer->DrawText( 255, 0, 0, 10, 40, "current camera : %s", camera_name.c_str() );
        


        renderer->FlipScreen();

        m_timer.Update();
        if( m_timer.IsReady() )
        {

        }

        // wxWidget framework specific !
        p_event.RequestMore( true );
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
    wxWidgetAdapter::GetInstance()->AdaptKeyboardOutputComboBox( &m_movements, m_mousekeyboardoutput_comboBox );

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
                dialog->SetData( "combo", m_mousekeyboardoutput_comboBox );

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
                dialog->SetData( "combo", m_mousekeyboardoutput_comboBox );

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
                dialog->SetData( "combo", m_mousekeyboardoutput_comboBox );

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
                dialog->SetData( "combo", m_mousekeyboardoutput_comboBox );

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
                dialog->SetData( "combo", m_mousekeyboardoutput_comboBox );

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
                dialog->SetData( "combo", m_mousekeyboardoutput_comboBox );

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
                dialog->SetData( "combo", m_mousekeyboardoutput_comboBox );

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
        dialog->SetData( "registers", &m_registers );
        dialog->EnableApplyButton();
        dialog->EnableSpecificButton0( "Add matrix" );
        dialog->EnableSpecificButton1( "Clear all" );
        wxWidgetAdapter::GetInstance()->AdaptMatrixStackEdition( &m_registers, dialog );
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

    wxWidgetAdapter::GetInstance()->AdaptMovementControlProps( mvt_name2, &m_registers, dialog );
    dialog->Show();
}

void BasicSceneMainFrame::OnMouseKeyboardOutputCombobox( wxCommandEvent& p_event )
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
    DrawSpace::Core::SingletonPlugin<Scripting>::GetInstance()->m_interface->TestCBCall();

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

void BasicSceneMainFrame::OnRegistersListItemActivated( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();

    wxString reg_name = m_registers_listCtrl->GetItemText( sel_index );
    wxCharBuffer buffer = reg_name.ToAscii();

    dsstring reg_name2 = buffer.data();

    RegisterEntry* register_entry = (RegisterEntry*)m_registers_listCtrl->GetItemData( sel_index );

    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Register properties" );
    wxWidgetAdapter::GetInstance()->AdaptRegProps( reg_name2, register_entry, dialog );
    dialog->Show();
}

void BasicSceneMainFrame::OnRegsListDeleteAllItems( wxListEvent& p_event )
{
    m_modreg_button->Enable( false );
    m_resetreg_button->Enable( false );
    m_regon_button->Enable( false );
    m_regoff_button->Enable( false );
}

void BasicSceneMainFrame::OnRegsListItemSelected( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();
    m_regslistctrl_currentindex = sel_index;

    
    m_modreg_button->Enable( true );

    RegisterEntry* reg_entry = (RegisterEntry*)m_registers_listCtrl->GetItemData( sel_index );

    if( REGISTER_VARIABLE == reg_entry->mode )
    {
        m_resetreg_button->Enable( true );
        m_regon_button->Enable( true );
        m_regoff_button->Enable( true );
    }
    else
    {
        m_resetreg_button->Enable( false );
        m_regon_button->Enable( false );
        m_regoff_button->Enable( false );
    }
}

void BasicSceneMainFrame::OnModRegButtonClicked( wxCommandEvent& p_event )
{
    RegisterEntry* reg_entry = (RegisterEntry*)m_registers_listCtrl->GetItemData( m_regslistctrl_currentindex );
    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Register properties modifications" );
    wxWidgetAdapter::GetInstance()->AdaptRegisterPropsModification( reg_entry, dialog );

    dialog->SetData( "reg_entry", reg_entry );
    dialog->EnableApplyButton();
    dialog->Show();
}

void BasicSceneMainFrame::OnResetRegButtonClicked( wxCommandEvent& p_event )
{
    RegisterEntry* reg_entry = (RegisterEntry*)m_registers_listCtrl->GetItemData( m_regslistctrl_currentindex );
    wxWidgetAdapter::GetInstance()->InitializeRegister( reg_entry );
}

void BasicSceneMainFrame::OnRegOnButtonClicked( wxCommandEvent& p_event )
{
    RegisterEntry* reg_entry = (RegisterEntry*)m_registers_listCtrl->GetItemData( m_regslistctrl_currentindex );
    reg_entry->state = true;
}

void BasicSceneMainFrame::OnRegOffButtonClicked( wxCommandEvent& p_event )
{
    RegisterEntry* reg_entry = (RegisterEntry*)m_registers_listCtrl->GetItemData( m_regslistctrl_currentindex );
    reg_entry->state = false;
}
