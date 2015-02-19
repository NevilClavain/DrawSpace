/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include <wx/wx.h>
#include "BasicSceneMainFrame.h"
#include "drawspace.h"
#include "adapters.h"
//#include "BasicSceneObjectPropertiesDialog.h"
#include "buildobjects.h"

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
m_current_camera( NULL ),
m_display_framerate( false ),
m_display_currentcamera( false ),
m_console_font( 8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false )
{
    m_transftype_button->Enable( false );
    m_transfoedit_button->Enable( false );
    m_control_button->Enable( false );
    m_cameraedit_button->Enable( false );
    m_modreg_button->Enable( false );
    m_resetreg_button->Enable( false );
    m_regon_button->Enable( false );
    m_regoff_button->Enable( false );

    m_consoleinput_textCtrl->SetFont( m_console_font );
    m_consoleoutput_textCtrl->SetFont( m_console_font );


    m_timercb = new TimerCallback( this, &BasicSceneMainFrame::on_timer );

    m_timer.AddTimer( "timer", 100, m_timercb );
    m_timer.SetTimerState( "timer", true );

    m_scripting = DrawSpace::Core::SingletonPlugin<Scripting>::GetInstance()->m_interface;

    m_scripting->Initialize();

    m_scripting_error_cb = new ScriptingErrorCallback( this, &BasicSceneMainFrame::on_scripting_error );
    m_scripting->RegisterScriptErrorHandler( m_scripting_error_cb );

    m_scripting_calls_cb = new ScriptingCallsCallback( this, &BasicSceneMainFrame::on_scripting_calls );
    m_scripting->RegisterScriptCallsHandler( m_scripting_calls_cb );

    ////////////////////////////////////

    // root; seul la creation de scenenodegraph est possible
    //m_scenegraphs_masks[0].push_back( CONTEXTMENU_NEWSCENENODEGRAPH );

    PopupMenuEntry pme_separator = { CONTEXTMENU_SEPARATOR, "" };
    PopupMenuEntry pme_edit = { CONTEXTMENU_EDIT, "Edit..." };

    PopupMenuEntry pme_newscenenodegraph = { CONTEXTMENU_NEWSCENENODEGRAPH, "New scenenodegraph..." };
    PopupMenuEntry pme_newspacebox = { CONTEXTMENU_NEWSPACEBOX, "New spacebox..." };
    PopupMenuEntry pme_newchunk = { CONTEXTMENU_NEWCHUNK, "New chunk..." };
    PopupMenuEntry pme_newinertbody = { CONTEXTMENU_NEWINERTBODY, "New inertbody..." };
    PopupMenuEntry pme_newcollider = { CONTEXTMENU_NEWCOLLIDER, "New collider..." };
    PopupMenuEntry pme_newrocket = { CONTEXTMENU_NEWROCKET, "New rocket..." };
    PopupMenuEntry pme_neworbit = { CONTEXTMENU_NEWORBIT, "New orbit..." };
    PopupMenuEntry pme_neworbiter = { CONTEXTMENU_NEWORBITER, "New orbiter..." };
    PopupMenuEntry pme_newplanet = { CONTEXTMENU_NEWPLANET, "New planet..." };
    PopupMenuEntry pme_newtransfo = { CONTEXTMENU_NEWTRANSFO, "New transformation..." };
    PopupMenuEntry pme_newcamera = { CONTEXTMENU_NEWCAMERA, "New camera point..." };
    PopupMenuEntry pme_newlinearmvt = { CONTEXTMENU_NEWLINEARMVT, "New linear movement..." };
    PopupMenuEntry pme_newcircularmvt = { CONTEXTMENU_NEWCIRCULARMVT, "New circular movement..." };
    PopupMenuEntry pme_newfpsmvt = { CONTEXTMENU_NEWFPSMVT, "New fps movement..." };
    PopupMenuEntry pme_newfreemvt = { CONTEXTMENU_NEWFREEMVT, "New free movement..." };
    PopupMenuEntry pme_newheadmvt = { CONTEXTMENU_NEWHEADMVT, "New head movement..." };
    PopupMenuEntry pme_newlonglatmvt = { CONTEXTMENU_NEWLONGLATMVT, "New longlat movement..." };
    PopupMenuEntry pme_newspectatormvt = { CONTEXTMENU_NEWSPECTATORMVT, "New spectator movement..." };


    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[DRAWSPACE_MASK].push_back( pme_newscenenodegraph );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newspacebox );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newchunk );

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_separator );

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newinertbody );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newcollider );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newrocket );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_neworbit );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newplanet );

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_separator );

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newtransfo );
    
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_separator );

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newlinearmvt );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newcircularmvt );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newfpsmvt );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newfreemvt );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newspectatormvt );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newspacebox );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newcollider );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_neworbit );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newplanet );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_separator );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newtransfo );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_separator );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newlinearmvt );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newcircularmvt );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newlonglatmvt );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newheadmvt );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_separator );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_edit );


    m_applybutton_clicked_cb = new CallBack<BasicSceneMainFrame, void, BasicSceneObjectPropertiesDialog*>( this, &BasicSceneMainFrame::on_applybutton_clicked );
    m_specificbutton0_clicked_cb = new CallBack<BasicSceneMainFrame, void, BasicSceneObjectPropertiesDialog*>( this, &BasicSceneMainFrame::on_specificbutton0_clicked );
    m_specificbutton1_clicked_cb = new CallBack<BasicSceneMainFrame, void, BasicSceneObjectPropertiesDialog*>( this, &BasicSceneMainFrame::on_specificbutton1_clicked );

}

void BasicSceneMainFrame::on_scripting_error( const dsstring& p_error )
{
    PrintOutputConsole( p_error );
    wxMessageBox( "Scripting syntax error", "Script error", wxICON_ERROR );
}

bool BasicSceneMainFrame::set_var_alias( const dsstring& p_source, dsstring& p_dest )
{
    bool status = true;
    if( p_source != "" )
    {
        if( m_registers.count( p_source ) > 0 )
        {
            p_dest = p_source;
        }
        else
        {
            wxMessageBox( "unknown register : " + p_source, "Script error", wxICON_ERROR );
            status = false;
        }
    }
    else
    {
        p_dest = "...";
    }

    return status;
}

void BasicSceneMainFrame::build_popupmenu( int p_level, wxMenu& p_menu )
{   
    for( size_t i = 0; i < m_scenegraphs_masks[p_level].size(); i++ )
    {
        if( CONTEXTMENU_SEPARATOR == m_scenegraphs_masks[p_level][i].id )
        {
            p_menu.AppendSeparator();
        }
        else
        {
            p_menu.Append( m_scenegraphs_masks[p_level][i].id, m_scenegraphs_masks[p_level][i].text.c_str() );
        }
    }
}

bool BasicSceneMainFrame::RegisterMovement( const dsstring& p_name, const MovementEntry& p_movement )
{
    if( m_movements.count( p_name ) > 0 )
    {
        wxMessageBox( "Movement with same name already exists : " + p_name, "Drawfront error", wxICON_ERROR );
        return false;
    }

    m_movements[p_name] = p_movement;

    wxWidgetAdapter::GetInstance()->AdaptMvtsList( &m_movements, m_mvts_listCtrl );
    wxWidgetAdapter::GetInstance()->AdaptKeyboardOutputComboBox( &m_movements, m_mousekeyboardoutput_comboBox );

    return true;
}

void BasicSceneMainFrame::on_scripting_calls( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring script_call_id = p_propertypool.GetPropValue<dsstring>( "script_call_id" );

    if( "global:print" == script_call_id )
    {
        dsstring text = p_propertypool.GetPropValue<dsstring>( "text" );
        PrintOutputConsole( text );
    }
    else if( "DrawSpace:DisplayFramerate" == script_call_id )
    {
        bool state = p_propertypool.GetPropValue<bool>( "state" );
        m_display_framerate = state;
    }
    else if( "DrawSpace:DisplayCurrentCamera" == script_call_id )
    {
        bool state = p_propertypool.GetPropValue<bool>( "state" );
        m_display_currentcamera = state;
    }
    else if( "DrawSpace:CreateSceneNodeGraph" == script_call_id )
    {
        dsstring alias = p_propertypool.GetPropValue<dsstring>( "name" );

        SceneNodeGraphEntry entry;

        entry.name = alias;
        entry.scenenodegraph = new SceneNodeGraph();
        entry.treeitemid = m_scenegraphs_treeCtrl->AppendItem( m_scenegraphs_root_item, alias.c_str(), SCENEGRAPH_ICON_INDEX );
        m_scenenodegraphs[entry.treeitemid.GetID()] = entry;

        m_scenegraphs_treeCtrl->ExpandAllChildren( m_scenegraphs_root_item );
    }
    else if( "SpaceBoxBuilder:BuildIt" == script_call_id )
    {
        Spacebox::Descriptor descriptor = p_propertypool.GetPropValue<Spacebox::Descriptor>( "descr" );

        MetadataScenegraphEntry entry;
        dsstring error_msg;
        bool status = BuildSpaceBox( descriptor, entry, error_msg );

        if( status )
        {
            m_scenegraph.RegisterNode( entry.node );

            m_metada_scenegraph[descriptor.scene_name] = entry;

            // call UpdateOutputQueue() for all passes
            
            for( size_t i = 0; i < m_ordered_configs.size(); i++ )
            {
                Pass* pass = dynamic_cast<Pass*>( m_ordered_configs[i] );
                if( pass )
                {
                    pass->GetRenderingQueue()->UpdateOutputQueue();
                }
            }

            wxWidgetAdapter::GetInstance()->AdaptScenegraphList( &m_scenegraph, m_scenegraph_listCtrl );
        }
        else
        {
            wxMessageBox( error_msg, "Script error", wxICON_ERROR );
        }
    }
    else if( "SpaceBoxBuilder:ClearMatrixStack" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "name" );

        if( m_metada_scenegraph.count( scene_name ) > 0 )
        {
            m_metada_scenegraph[scene_name].matrix_stack_descr.clear();
        }
        else
        {
            wxMessageBox( "Unknown spacebox scene name", "Script error", wxICON_ERROR );
        }
    }
    else if( "SpaceBoxBuilder:AddMatrix" == script_call_id )
    {
        dsstring mat_type = p_propertypool.GetPropValue<dsstring>( "mat_type" );

        dsreal x, y, z, angle;
        dsstring xreg, yreg, zreg, anglereg;
        TransformationMatrixDescriptor mat_descr;
        
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "name" );

        if( m_metada_scenegraph.count( scene_name ) > 0 )
        {           
            if( "scaling" == mat_type )
            {
                x = p_propertypool.GetPropValue<dsreal>( "x" );
                y = p_propertypool.GetPropValue<dsreal>( "y" );
                z = p_propertypool.GetPropValue<dsreal>( "z" );

                xreg = p_propertypool.GetPropValue<dsstring>( "xreg" );
                yreg = p_propertypool.GetPropValue<dsstring>( "yreg" );
                zreg = p_propertypool.GetPropValue<dsstring>( "zreg" );

                mat_descr.ope = TRANSFORMATIONMATRIX_SCALE;

                mat_descr.arg.scale_vals_link[0].value = x;
                if( !set_var_alias( xreg, mat_descr.arg.scale_vals_link[0].var_alias ) )
                {
                    return;
                }


                mat_descr.arg.scale_vals_link[1].value = y;
                if( !set_var_alias( yreg, mat_descr.arg.scale_vals_link[1].var_alias ) )
                {
                    return;
                }
                
                mat_descr.arg.scale_vals_link[2].value = z;
                if( !set_var_alias( zreg, mat_descr.arg.scale_vals_link[2].var_alias ) )
                {
                    return;
                }

                mat_descr.arg.translation_vals_link[0].var_alias = "...";
                mat_descr.arg.translation_vals_link[1].var_alias = "...";
                mat_descr.arg.translation_vals_link[2].var_alias = "...";
                mat_descr.arg.translation_vals_link[0].value = 0.0;
                mat_descr.arg.translation_vals_link[1].value = 0.0;
                mat_descr.arg.translation_vals_link[2].value = 0.0;


                mat_descr.arg.rotation_vals_link[0].var_alias = "...";
                mat_descr.arg.rotation_vals_link[1].var_alias = "...";
                mat_descr.arg.rotation_vals_link[2].var_alias = "...";
                mat_descr.arg.rotation_vals_link[0].value = 0.0;
                mat_descr.arg.rotation_vals_link[1].value = 0.0;
                mat_descr.arg.rotation_vals_link[2].value = 0.0;

                mat_descr.arg.angle_val_link.var_alias = "...";
                mat_descr.arg.angle_val_link.value = 0.0;


                m_metada_scenegraph[scene_name].matrix_stack_descr.push_back( mat_descr );
            }
            else if( "translation" == mat_type )
            {
                x = p_propertypool.GetPropValue<dsreal>( "x" );
                y = p_propertypool.GetPropValue<dsreal>( "y" );
                z = p_propertypool.GetPropValue<dsreal>( "z" );

                xreg = p_propertypool.GetPropValue<dsstring>( "xreg" );
                yreg = p_propertypool.GetPropValue<dsstring>( "yreg" );
                zreg = p_propertypool.GetPropValue<dsstring>( "zreg" );

                mat_descr.ope = TRANSFORMATIONMATRIX_TRANSLATION;

                mat_descr.arg.translation_vals_link[0].value = x;
                if( !set_var_alias( xreg, mat_descr.arg.translation_vals_link[0].var_alias ) )
                {
                    return;
                }

                mat_descr.arg.translation_vals_link[1].value = y;
                if( !set_var_alias( yreg, mat_descr.arg.translation_vals_link[1].var_alias ) )
                {
                    return;
                }
                
                mat_descr.arg.translation_vals_link[2].value = z;
                if( !set_var_alias( zreg, mat_descr.arg.translation_vals_link[2].var_alias ) )
                {
                    return;
                }


                mat_descr.arg.scale_vals_link[0].var_alias = "...";
                mat_descr.arg.scale_vals_link[1].var_alias = "...";
                mat_descr.arg.scale_vals_link[2].var_alias = "...";
                mat_descr.arg.scale_vals_link[0].value = 0.0;
                mat_descr.arg.scale_vals_link[1].value = 0.0;
                mat_descr.arg.scale_vals_link[2].value = 0.0;

                mat_descr.arg.rotation_vals_link[0].var_alias = "...";
                mat_descr.arg.rotation_vals_link[1].var_alias = "...";
                mat_descr.arg.rotation_vals_link[2].var_alias = "...";
                mat_descr.arg.rotation_vals_link[0].value = 0.0;
                mat_descr.arg.rotation_vals_link[1].value = 0.0;
                mat_descr.arg.rotation_vals_link[2].value = 0.0;

                mat_descr.arg.angle_val_link.var_alias = "...";
                mat_descr.arg.angle_val_link.value = 0.0;

                m_metada_scenegraph[scene_name].matrix_stack_descr.push_back( mat_descr );
            }
            else if( "rotation" == mat_type )
            {
                x = p_propertypool.GetPropValue<dsreal>( "x" );
                y = p_propertypool.GetPropValue<dsreal>( "y" );
                z = p_propertypool.GetPropValue<dsreal>( "z" );
                angle = p_propertypool.GetPropValue<dsreal>( "angle" );

                xreg = p_propertypool.GetPropValue<dsstring>( "xreg" );
                yreg = p_propertypool.GetPropValue<dsstring>( "yreg" );
                zreg = p_propertypool.GetPropValue<dsstring>( "zreg" );
                anglereg = p_propertypool.GetPropValue<dsstring>( "areg" );

                mat_descr.ope = TRANSFORMATIONMATRIX_ROTATION;

                mat_descr.arg.rotation_vals_link[0].value = x;
                if( !set_var_alias( xreg, mat_descr.arg.rotation_vals_link[0].var_alias ) )
                {
                    return;
                }

                mat_descr.arg.rotation_vals_link[1].value = y;
                if( !set_var_alias( yreg, mat_descr.arg.rotation_vals_link[1].var_alias ) )
                {
                    return;
                }
                
                mat_descr.arg.rotation_vals_link[2].value = z;
                if( !set_var_alias( zreg, mat_descr.arg.rotation_vals_link[2].var_alias ) )
                {
                    return;
                }

                mat_descr.arg.angle_val_link.value = angle;
                if( !set_var_alias( anglereg, mat_descr.arg.angle_val_link.var_alias ) )
                {
                    return;
                }


                mat_descr.arg.scale_vals_link[0].var_alias = "...";
                mat_descr.arg.scale_vals_link[1].var_alias = "...";
                mat_descr.arg.scale_vals_link[2].var_alias = "...";
                mat_descr.arg.scale_vals_link[0].value = 0.0;
                mat_descr.arg.scale_vals_link[1].value = 0.0;
                mat_descr.arg.scale_vals_link[2].value = 0.0;

                mat_descr.arg.translation_vals_link[0].var_alias = "...";
                mat_descr.arg.translation_vals_link[1].var_alias = "...";
                mat_descr.arg.translation_vals_link[2].var_alias = "...";
                mat_descr.arg.translation_vals_link[0].value = 0.0;
                mat_descr.arg.translation_vals_link[1].value = 0.0;
                mat_descr.arg.translation_vals_link[2].value = 0.0;


                m_metada_scenegraph[scene_name].matrix_stack_descr.push_back( mat_descr );
            }        
            else
            {
                wxMessageBox( "Bad matrix type", "Script error", wxICON_ERROR );
            }            
        }
        else
        {
            wxMessageBox( "Unknown spacebox scene name", "Script error", wxICON_ERROR );
        }
    }
    else if( "DrawSpace:CreateConstRegister" == script_call_id )
    {
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        dsreal reg_val = p_propertypool.GetPropValue<dsreal>( "reg_val" );

        BasicSceneMainFrame::RegisterEntry register_entry;
        memset( &register_entry, 0, sizeof( BasicSceneMainFrame::RegisterEntry ) );

        register_entry.mode = BasicSceneMainFrame::REGISTER_CONSTANT;
        register_entry.const_value = reg_val;
        register_entry.id = wxWidgetAdapter::GetInstance()->m_register_index++;
        wxWidgetAdapter::GetInstance()->InitializeRegister( &register_entry );

        m_registers[reg_name] = register_entry;
        wxWidgetAdapter::GetInstance()->AdaptRegistersList( &m_registers, m_registers_listCtrl );
    }
    else if( "DrawSpace:ModifyConstRegisterValue" == script_call_id )
    {
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        dsreal reg_val = p_propertypool.GetPropValue<dsreal>( "reg_val" );

        if( m_registers.count( reg_name ) > 0 )
        {
            if( REGISTER_CONSTANT == m_registers[reg_name].mode )
            {
                m_registers[reg_name].const_value = reg_val;
            }
            else
            {
                wxMessageBox( "Specified register is not a constant type : " + reg_name, "Script error", wxICON_ERROR );
            }
        }
        else
        {
            wxMessageBox( "Unknown register name : " + reg_name, "Script error", wxICON_ERROR );
        }
    }
    else if( "DrawSpace:CreateVarRegister" == script_call_id )
    {
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        dsstring var_mode = p_propertypool.GetPropValue<dsstring>( "var_mode" );
        dsreal init_val = p_propertypool.GetPropValue<dsreal>( "init_val" );
        dsreal speed = p_propertypool.GetPropValue<dsreal>( "speed" );
        dsreal range_inf = p_propertypool.GetPropValue<dsreal>( "range_inf" );
        dsreal range_sup = p_propertypool.GetPropValue<dsreal>( "range_sup" );

        BasicSceneMainFrame::RegisterEntry register_entry;
        memset( &register_entry, 0, sizeof( BasicSceneMainFrame::RegisterEntry ) );

        register_entry.mode = BasicSceneMainFrame::REGISTER_VARIABLE;

        if( "translation_simple" == var_mode )
        {
            register_entry.variable_mode = BasicSceneMainFrame::REGISTER_VARIABLE_TRANSLATION_SIMPLE;
        }
        else if( "translation_roundtrip" == var_mode )
        {
            register_entry.variable_mode = BasicSceneMainFrame::REGISTER_VARIABLE_TRANSLATION_ROUNDTRIP;
        }
        else if( "angular_simple" == var_mode )
        {
            register_entry.variable_mode = BasicSceneMainFrame::REGISTER_VARIABLE_ANGULAR_SIMPLE;
        }
        else if( "angular_roundtrip" == var_mode )
        {
            register_entry.variable_mode = BasicSceneMainFrame::REGISTER_VARIABLE_ANGULAR_ROUNDTRIP;
        }

        register_entry.variable_initial_value = init_val;
        register_entry.variable_speed = speed;
        register_entry.variable_range_inf = range_inf;
        register_entry.variable_range_sup = range_sup;

        register_entry.id = wxWidgetAdapter::GetInstance()->m_register_index++;
        wxWidgetAdapter::GetInstance()->InitializeRegister( &register_entry );
        m_registers[reg_name] = register_entry;
        wxWidgetAdapter::GetInstance()->AdaptRegistersList( &m_registers, m_registers_listCtrl );

    }
    else if( "DrawSpace:ResetVarRegister" == script_call_id )
    {
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );

        if( m_registers.count( reg_name ) > 0 )
        {
            if( REGISTER_VARIABLE == m_registers[reg_name].mode )
            {
                wxWidgetAdapter::GetInstance()->InitializeRegister( &m_registers[reg_name] );      
            }
            else
            {
                wxMessageBox( "Specified register is not a variable type : " + reg_name, "Script error", wxICON_ERROR );
            }
        }
        else
        {
            wxMessageBox( "Unknown register name : " + reg_name, "Script error", wxICON_ERROR );
        }
    }
    else if( "DrawSpace:SetVarRegisterState" == script_call_id )
    {
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        bool state = p_propertypool.GetPropValue<bool>( "state" );

        if( m_registers.count( reg_name ) > 0 )
        {
            if( REGISTER_VARIABLE == m_registers[reg_name].mode )
            {
                m_registers[reg_name].state = state;
            }
            else
            {
                wxMessageBox( "Specified register is not a variable type : " + reg_name, "Script error", wxICON_ERROR );
            }
        }
        else
        {
            wxMessageBox( "Unknown register name : " + reg_name, "Script error", wxICON_ERROR );
        }
    }
    else if( "DrawSpace:SetVarRegisterInitVal" == script_call_id )
    {
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        dsreal init_val = p_propertypool.GetPropValue<dsreal>( "init_val" );

        if( m_registers.count( reg_name ) > 0 )
        {
            if( REGISTER_VARIABLE == m_registers[reg_name].mode )
            {
                m_registers[reg_name].variable_initial_value = init_val;
            }
            else
            {
                wxMessageBox( "Specified register is not a variable type : " + reg_name, "Script error", wxICON_ERROR );
            }
        }
        else
        {
            wxMessageBox( "Unknown register name : " + reg_name, "Script error", wxICON_ERROR );
        }
    }
    else if( "DrawSpace:SetVarRegisterSpeed" == script_call_id )
    {
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        dsreal speed = p_propertypool.GetPropValue<dsreal>( "speed" );

        if( m_registers.count( reg_name ) > 0 )
        {
            if( REGISTER_VARIABLE == m_registers[reg_name].mode )
            {
                m_registers[reg_name].variable_speed = speed;
            }
            else
            {
                wxMessageBox( "Specified register is not a variable type : " + reg_name, "Script error", wxICON_ERROR );
            }
        }
        else
        {
            wxMessageBox( "Unknown register name : " + reg_name, "Script error", wxICON_ERROR );
        }
    }
    else if( "DrawSpace:SetVarRegisterRange" == script_call_id )
    {
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        dsreal range_min = p_propertypool.GetPropValue<dsreal>( "range_min" );
        dsreal range_max = p_propertypool.GetPropValue<dsreal>( "range_max" );

        if( m_registers.count( reg_name ) > 0 )
        {
            if( REGISTER_VARIABLE == m_registers[reg_name].mode )
            {
                m_registers[reg_name].variable_range_inf = range_min;
                m_registers[reg_name].variable_range_sup = range_max;
            }
            else
            {
                wxMessageBox( "Specified register is not a variable type : " + reg_name, "Script error", wxICON_ERROR );
            }
        }
        else
        {
            wxMessageBox( "Unknown register name : " + reg_name, "Script error", wxICON_ERROR );
        }
    }
    else if( "MovementBuilder:Lua_BuildIt" == script_call_id )
    {
        dsstring type_name = p_propertypool.GetPropValue<dsstring>( "type" );
        dsstring name = p_propertypool.GetPropValue<dsstring>( "name" );

        BasicSceneMainFrame::MovementEntry movement_entry;

        movement_entry.speed_control_source = BasicSceneMainFrame::MOVEMENTCONTROLSOURCE_KEYBMOUSE;
        movement_entry.yaw_control_source = BasicSceneMainFrame::MOVEMENTCONTROLSOURCE_KEYBMOUSE;
        movement_entry.pitch_control_source = BasicSceneMainFrame::MOVEMENTCONTROLSOURCE_KEYBMOUSE;
        movement_entry.roll_control_source = BasicSceneMainFrame::MOVEMENTCONTROLSOURCE_KEYBMOUSE;
        movement_entry.theta_pos_mouse = 0.0;
        movement_entry.phi_pos_mouse = 0.0;

        if( "Linear" == type_name )
        {
            Vector init_pos = p_propertypool.GetPropValue<Vector>( "initpos" );
            Vector direction = p_propertypool.GetPropValue<Vector>( "direction" );

            dsreal theta = p_propertypool.GetPropValue<dsreal>( "theta" );
            dsreal phi = p_propertypool.GetPropValue<dsreal>( "phi" );

            LinearMovement* linear_mvt = new LinearMovement();
            linear_mvt->Init( init_pos, direction, theta, phi );
                       
            movement_entry.movement = linear_mvt;
        }
        else if( "Circular" == type_name )
        {
            Vector center_pos = p_propertypool.GetPropValue<Vector>( "centerpos" );
            Vector delta_center = p_propertypool.GetPropValue<Vector>( "deltacenter" );
            Vector rot_axis = p_propertypool.GetPropValue<Vector>( "rotationaxis" );

            dsreal init_angle = p_propertypool.GetPropValue<dsreal>( "angle" );
            dsreal theta = p_propertypool.GetPropValue<dsreal>( "theta" );
            dsreal phi = p_propertypool.GetPropValue<dsreal>( "phi" );

            CircularMovement* circular_mvt = new CircularMovement();
            circular_mvt->Init( center_pos, delta_center, rot_axis, init_angle, theta, phi );

            movement_entry.movement = circular_mvt;
        }
        else if( "FPS" == type_name )
        {
            Vector init_pos = p_propertypool.GetPropValue<Vector>( "initpos" );
            dsreal init_yaw = p_propertypool.GetPropValue<dsreal>( "yaw" );
            dsreal init_pitch = p_propertypool.GetPropValue<dsreal>( "pitch" );

            FPSMovement* fps_mvt = new FPSMovement();
            fps_mvt->Init( init_pos, init_yaw, init_pitch );
            
            movement_entry.movement = fps_mvt;
        }
        else if( "Free" == type_name )
        {
            Vector init_pos = p_propertypool.GetPropValue<Vector>( "initpos" );

            FreeMovement* free_mvt = new FreeMovement();
            free_mvt->Init( init_pos );

            movement_entry.movement = free_mvt;            
        }
        else if( "Head" == type_name )
        {
            Vector head_pos = p_propertypool.GetPropValue<Vector>( "initpos" );
            dsreal scale_factor = p_propertypool.GetPropValue<dsreal>( "scalefactor" );
            dsreal ref_force = p_propertypool.GetPropValue<dsreal>( "refforce" );

            HeadMovement* head_mvt = new HeadMovement();
            head_mvt->Init( scale_factor, ref_force, head_pos );

            movement_entry.movement = head_mvt;            
        }
        else if( "Spectator" == type_name )
        {
            dsreal scale_pos = p_propertypool.GetPropValue<dsreal>( "scalefactor" );
            long period = p_propertypool.GetPropValue<long>( "period" );
            bool orbiter_link = p_propertypool.GetPropValue<bool>( "attachorbiter" );

            SpectatorMovement* spectator_mvt = new SpectatorMovement();
            spectator_mvt->Init( scale_pos, period, orbiter_link );

            movement_entry.movement = spectator_mvt;
        }
        else if( "Longlat" == type_name )
        {
            dsreal init_longit = p_propertypool.GetPropValue<dsreal>( "longitud" );
            dsreal init_latit = p_propertypool.GetPropValue<dsreal>( "latitud" );
            dsreal init_altitud = p_propertypool.GetPropValue<dsreal>( "altitud" );

            dsreal init_theta = p_propertypool.GetPropValue<dsreal>( "theta" );
            dsreal init_phi = p_propertypool.GetPropValue<dsreal>( "phi" );

            LongLatMovement* longlat_mvt = new LongLatMovement();
            longlat_mvt->Init( init_longit, init_latit, init_altitud, init_theta, init_phi );

            movement_entry.movement = longlat_mvt;
        }
        else
        {
            wxMessageBox( "Unknown movement class name : " + type_name, "Script error", wxICON_ERROR );
            return;
        }

        RegisterMovement( name, movement_entry );
    }
    else if( "TransformationNodeBuilder:LinkTo" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        dsstring scenegraph_name = p_propertypool.GetPropValue<dsstring>( "scenegraph_name" );
        dsstring parent_name = p_propertypool.GetPropValue<dsstring>( "parent_name" );
        BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );

        wxTreeItemId parent_tree_item;
        void* parent_id = NULL;

        // search for scenenodegraph

        bool scene_found = false;
        SceneNodeGraphEntry scenenodegraph_entry;

        for( std::map<void*, SceneNodeGraphEntry>::iterator it = m_scenenodegraphs.begin(); it != m_scenenodegraphs.end(); ++it )
        {
            if( it->second.name == scenegraph_name )
            {
                scenenodegraph_entry = it->second;
                scene_found = true;                
                break;
            }
        }

        bool parent_found = false;
        BaseSceneNode* parent = NULL;


        for( std::map<void*, DrawSpace::Core::BaseSceneNode*>::iterator it = m_tree_nodes.begin(); it != m_tree_nodes.end(); ++it )
        {
            dsstring node_scenename;
            it->second->GetSceneName( node_scenename );

            if( node_scenename == parent_name )
            {
                parent_found = true;
                parent = it->second;
                parent_id = it->first;
                break;
            }
        }

        if( !parent_found )
        {
            for( std::map<void*, SceneNodeGraphEntry>::iterator it = m_scenenodegraphs.begin(); it != m_scenenodegraphs.end(); ++it )
            {
                if( it->second.name == parent_name )
                {
                    parent_found = true;
                    parent_id = it->first;
                    break;
                }
            }
        }

        if( !scene_found )
        {
            wxMessageBox( "Transformation node, unknown scenegraph name : " + scenegraph_name, "Script error", wxICON_ERROR );
            return;           
        }

        else if( !parent_found )
        {
            wxMessageBox( "Transformation node, unknown parent name : " + parent_name, "Script error", wxICON_ERROR );
            return;
        }

        SceneNode<Transformation>* transfo_node = static_cast<SceneNode<Transformation>*>( node );


        scenenodegraph_entry.scenenodegraph->RegisterNode( node );

        if( parent )
        {            
            transfo_node->LinkTo( parent );
            parent_tree_item = m_transformation_nodes[parent_id].treeitemid;
        }
        else
        {
            scenenodegraph_entry.scenenodegraph->AddNode( node );
            parent_tree_item = scenenodegraph_entry.treeitemid;
        }

        
        // GUI : add item in the tree
        wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( parent_tree_item, scene_name.c_str(), TRANSFO_ICON_INDEX );
        m_scenegraphs_treeCtrl->ExpandAllChildren( parent_tree_item );
        
        // record the new transformation node and associated metadata

        BasicSceneMainFrame::TransformationNodeEntry t_entry;

        t_entry.name = scene_name;
        t_entry.transformation = transfo_node;
        t_entry.treeitemid = treeitemid;

        /*
        BasicSceneMainFrame::TransformationMatrixDescriptor transfdescr;
        transfdescr.ope = BasicSceneMainFrame::TRANSFORMATIONMATRIX_IDENTITY;

        t_entry.matrix_stack_descr.push_back( transfdescr );
        */

        m_transformation_nodes[t_entry.treeitemid.GetID()] = t_entry;
        m_tree_nodes[t_entry.treeitemid.GetID()] = transfo_node;
    }
}

void BasicSceneMainFrame::ExecStartupScript( const dsstring& p_scriptfilepath )
{
    m_scripting->ExecFile( p_scriptfilepath.c_str() );
}

void BasicSceneMainFrame::PrintOutputConsole( const dsstring& p_text )
{
    m_consoleoutput_textCtrl->AppendText( wxString( p_text.c_str() ) );
    m_consoleoutput_textCtrl->AppendText( wxString( "\n" ) );
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

                    Transformation matrix_stack;

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


void BasicSceneMainFrame::compute_transformnodes( void )
{
    for( std::map<void*, TransformationNodeEntry>::iterator it = m_transformation_nodes.begin(); it != m_transformation_nodes.end(); ++it )
    {
        Matrix::ConfigurationInfo mci;

        std::vector<Matrix> mat_list;
        it->second.transformation->GetContent()->GetMatrixChain( mat_list );

        for( size_t i = 0; i < mat_list.size(); i++ )
        {
            Matrix::ConfigurationInfo mci;
            Matrix updated_mat;
            mat_list[i].GetConfigInfos( mci );

            Vector values = mci.values;
            for( size_t j = 0; j < 4; j++ )
            {                        
                if( mci.metadatas[j] != "" && m_registers.count( mci.metadatas[j] ) > 0 )
                {                           
                    values[j] = m_registers[mci.metadatas[j]].current_value;                                
                }                        
            }

            switch( mci.type )
            {
                case Matrix::CONFIG_TRANSLATION:
                {
                    updated_mat.Translation( values[0], values[1], values[2] );
                }
                break;

                case Matrix::CONFIG_SCALING:
                {
                    updated_mat.Scale( values[0], values[1], values[2] );
                }
                break;

                case Matrix::CONFIG_ROTATION:
                {
                    updated_mat.Rotation( Vector( values[0], values[1], values[2], 1.0 ), values[3] );
                }
                break;
            }

            updated_mat.SetMetaData( 0, mci.metadatas[0] );
            updated_mat.SetMetaData( 1, mci.metadatas[1] );
            updated_mat.SetMetaData( 2, mci.metadatas[2] );
            updated_mat.SetMetaData( 3, mci.metadatas[3] );

            it->second.transformation->GetContent()->UpdateMatrix( i, updated_mat );
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

        compute_transformnodes();
                
        //m_scenegraph.ComputeTransformations( m_timer );


        for( std::map<void*, SceneNodeGraphEntry>::iterator it = m_scenenodegraphs.begin(); it != m_scenenodegraphs.end(); ++it )
        {
            it->second.scenenodegraph->ComputeTransformations( m_timer );
        }

        for( size_t i = 0; i < m_ordered_configs.size(); i++ )
        {
            Pass* pass = dynamic_cast<Pass*>( m_ordered_configs[i] );
            if( pass )
            {
                pass->GetRenderingQueue()->Draw();
            }
        }

        if( m_display_framerate )
        {
            renderer->DrawText( 255, 0, 0, 10, 20, "%d fps", m_timer.GetFPS() );
        }

        if( m_display_currentcamera )
        {            
            dsstring camera_name;
            m_scenegraph.GetCurrentCameraName( camera_name );
            if( "" == camera_name )
            {
                camera_name = "...";
            }

            renderer->DrawText( 255, 0, 0, 10, 40, "current camera : %s", camera_name.c_str() );
        }
        


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

    

    
    wxImageList* pImageList = new wxImageList( DFRONT_ICON_DIM, DFRONT_ICON_DIM, TRUE );


    wxBitmap bmp_camera( "icon_camera.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_chunk( "icon_chunk.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_collider( "icon_collider.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_inertbody( "icon_inertbody.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_movement( "icon_movement.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_orbit( "icon_orbit.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_orbiter( "icon_orbiter.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_planet( "icon_planet.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_rocket( "icon_rocket.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_spacebox( "icon_spacebox.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_transfo( "icon_transfo.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_scenegraph( "icon_scenegraph.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_drawspace( "icon_drawspace.bmp", wxBITMAP_TYPE_BMP );


    
    pImageList->Add( bmp_camera );
    pImageList->Add( bmp_chunk );
    pImageList->Add( bmp_collider );
    pImageList->Add( bmp_inertbody );
    pImageList->Add( bmp_movement );
    pImageList->Add( bmp_orbit );
    pImageList->Add( bmp_orbiter );
    pImageList->Add( bmp_planet );
    pImageList->Add( bmp_rocket );
    pImageList->Add( bmp_spacebox );
    pImageList->Add( bmp_transfo );
    pImageList->Add( bmp_scenegraph );
    pImageList->Add( bmp_drawspace );
    

    m_scenegraphs_treeCtrl->AssignImageList( pImageList );


    /*
    wxTreeItemId ti_root = m_treeCtrl1->AddRoot( "prout0", SPACEBOX_ICON_INDEX );
    wxTreeItemId ti_sb0 = m_treeCtrl1->AppendItem( ti_root, "sub_prout0", INERTBODY_ICON_INDEX );
    wxTreeItemId ti_sb1 = m_treeCtrl1->AppendItem( ti_root, "sub_prout1", PLANET_ICON_INDEX );
    wxTreeItemId ti_sb10 = m_treeCtrl1->AppendItem( ti_sb1, "sub_prout10", ORBITER_ICON_INDEX );
    */
    
    m_scenegraphs_root_item = m_scenegraphs_treeCtrl->AddRoot( "DrawSpace", DRAWSPACE_ICON_INDEX );
}

void BasicSceneMainFrame::OnAssetsListItemActivated( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();

    Asset* asset = (Asset*)m_assets_listCtrl->GetItemData( sel_index );

    Texture* texture = dynamic_cast<Texture*>( asset );
    if( texture )
    {
        BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Texture properties" );
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
                
                dialog->SetData( "frame", this );
                wxWidgetAdapter::GetInstance()->AdaptLinearMvtCreationProps( dialog );
                dialog->EnableApplyButton();
                dialog->Show();
            }
            break;

        case 1:
            // Circular
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Circular movement creation" );

                dialog->SetData( "frame", this );
                wxWidgetAdapter::GetInstance()->AdaptCircularMvtCreationProps( dialog );
                dialog->EnableApplyButton();
                dialog->Show();
            }
            break;


        case 2:
            // FPS
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "FPS movement creation" );

                dialog->SetData( "frame", this );
                wxWidgetAdapter::GetInstance()->AdaptFPSMvtCreationProps( dialog );
                dialog->EnableApplyButton();
                dialog->Show();
            }
            break;

        case 3:
            // Free
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Free movement creation" );

                dialog->SetData( "frame", this );
                wxWidgetAdapter::GetInstance()->AdaptFreeMvtCreationProps( dialog );
                dialog->EnableApplyButton();
                dialog->Show();
            }
            break;

        case 4:
            // Head
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Head movement creation" );

                dialog->SetData( "frame", this );
                wxWidgetAdapter::GetInstance()->AdaptHeadMvtCreationProps( dialog );
                dialog->EnableApplyButton();
                dialog->Show();
            }
            break;

        case 5:
            // Spectator
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Spectator movement creation" );

                dialog->SetData( "frame", this );
                wxWidgetAdapter::GetInstance()->AdaptSpectatorMvtCreationProps( dialog );
                dialog->EnableApplyButton();
                dialog->Show();
            }
            break;

        case 6:
            // LongLat
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Longlat movement creation" );

                dialog->SetData( "frame", this );

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
    /*
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
    */
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

void BasicSceneMainFrame::OnPopupClick(wxCommandEvent& p_evt)
{   
 	switch( p_evt.GetId() ) 
    {
 		case CONTEXTMENU_NEWSCENENODEGRAPH:
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Scenenodegraph creation" );

                wxWidgetAdapter::GetInstance()->AdaptScenegraphnodeCreationProps( dialog );

                dialog->SetData( "scenenodegraphs_map", &m_scenenodegraphs );
                dialog->SetData( "scenegraphs_treeCtrl", m_scenegraphs_treeCtrl );
                dialog->SetData( "scenegraphs_root_item", &m_scenegraphs_root_item );
                dialog->EnableApplyButton();
                dialog->Show();
                break;
            }
 			break;

        case CONTEXTMENU_NEWTRANSFO:
            {
                BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Transformation node creation" );

                wxWidgetAdapter::GetInstance()->AdaptTransfonodeCreationProps( dialog );

                dialog->SetData( "scenenodegraphs_map", &m_scenenodegraphs );
                dialog->SetData( "last_clicked_treeitem", &m_last_clicked_treeitem );
                dialog->SetData( "scenegraphs_treeCtrl", m_scenegraphs_treeCtrl );
                dialog->SetData( "transformation_nodes_map", &m_transformation_nodes );
                dialog->SetData( "tree_nodes_map", &m_tree_nodes );

                dialog->EnableApplyButton();
                dialog->Show();
            }
            break;

        case CONTEXTMENU_EDIT:
            {
                void* id = m_last_clicked_treeitem.GetID();

                if( m_transformation_nodes.count( id ) > 0 )
                {
                    TransformationNodeEntry* tne = &( m_transformation_nodes[id] );

                    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Transformation node edition" );

                    dialog->SetData( "registers", &m_registers );
                    dialog->SetData( "transfo_node_entry", tne );
                    dialog->EnableApplyButton();
                    dialog->EnableSpecificButton0( "Add matrix" );
                    dialog->EnableSpecificButton1( "Clear all" );
                    wxWidgetAdapter::GetInstance()->AdaptMatrixStackEdition( &m_registers, tne, dialog );
                    dialog->Show();
                }
            }
            break;

        case CONTEXTMENU_NEWSPACEBOX:
            {

             
                DIALOG_DECLARE( "Spacebox node creation" )
                DIALOG_APPENDROOT_STRING( "scene name", "" )
                DIALOG_APPENDROOT_INTEGER( "the beast", 666 )
                DIALOG_APPENDROOT_FLOAT( "pi", 3.1415927 )
                DIALOG_APPENDROOT_ENUM( "list", insert_void_choice( get_textures_list() ) )

                /*

                DIALOG_APPENDROOT_NODE( "a pass", root_pass )
                DIALOG_APPENDNODE_INTEGER( root_pass, "order", 200 )
                DIALOG_APPENDNODE_NODE( root_pass, "textures", textures_pass )

                DIALOG_APPENDNODE_STRING( textures_pass, "textures alias", "" )
                DIALOG_APPENDNODE_ENUM( textures_pass, "texture resource name", get_shaders_list() );



                
                DIALOG_BUILD_LABELS( 32, "texture_%d", textures )

                
                DIALOG_APPENDNODE_ITERATE( textures_pass, 45, DIALOG_APPENDNODE_INTEGER, textures )

                */


                DIALOG_APPLY

                DIALOG_SPECIFIC0( "Add new pass" )



                DIALOG_SHOW
            }
            break;
 	}
 }

void BasicSceneMainFrame::OnSceneNodeGraphsListRightClick( wxTreeEvent& p_event )
{	
 	wxMenu mnu;
 	
    wxTreeItemId item = p_event.GetItem();

    m_last_clicked_treeitem = item;

    if( item.GetID() == m_scenegraphs_root_item.GetID() )
    {
        build_popupmenu( DRAWSPACE_MASK, mnu );
    }
    else
    {
        
        if( m_scenenodegraphs.count( item.GetID() ) > 0 )
        {
            // clicked item belong to a registered scenenodegraph...
            build_popupmenu( SCENEGRAPH_MASK, mnu );
        }
        else if( m_transformation_nodes.count( item.GetID() ) > 0 )
        {
            build_popupmenu( TRANSFO_MASK, mnu );
        }
    }

 	mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&BasicSceneMainFrame::OnPopupClick, NULL, this );
 	PopupMenu(&mnu);
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

void BasicSceneMainFrame::OnConsoleInSendButtonClicked( wxCommandEvent& p_event )
{
    m_scripting->ExecChunk( m_consoleinput_textCtrl->GetValue().c_str() );
}

void BasicSceneMainFrame::OnConsoleInClearButtonClicked( wxCommandEvent& p_event )
{
    m_consoleinput_textCtrl->Clear();
}

void BasicSceneMainFrame::OnConsoleInLoadButtonClicked( wxCommandEvent& p_event )
{
}

wxArrayString BasicSceneMainFrame::get_passes_list( void )
{
    wxArrayString availables_passes_labels;
    std::map<dsstring, DrawSpace::Core::Configurable*> configs;

    ConfigsBase::GetInstance()->GetConfigsInstancesList( configs );
    for( std::map<dsstring, DrawSpace::Core::Configurable*>::iterator it = configs.begin(); it != configs.end(); ++it )
    {
        Pass* pass = dynamic_cast<Pass*>( it->second );
        if( pass )
        {
            dsstring pass_name;            
            availables_passes_labels.Add( it->first.c_str() );
        }
    }
    return availables_passes_labels;
}

wxArrayString BasicSceneMainFrame::get_finalpasses_list( void )
{
    wxArrayString availables_passes_labels;
    std::map<dsstring, DrawSpace::Core::Configurable*> configs;

    ConfigsBase::GetInstance()->GetConfigsInstancesList( configs );
    for( std::map<dsstring, DrawSpace::Core::Configurable*>::iterator it = configs.begin(); it != configs.end(); ++it )
    {
        FinalPass* pass = dynamic_cast<FinalPass*>( it->second );
        if( pass )
        {
            dsstring pass_name;            
            availables_passes_labels.Add( it->first.c_str() );
        }
    }
    return availables_passes_labels;
}

wxArrayString BasicSceneMainFrame::get_intermediatepasses_list( void )
{
    wxArrayString availables_passes_labels;
    std::map<dsstring, DrawSpace::Core::Configurable*> configs;

    ConfigsBase::GetInstance()->GetConfigsInstancesList( configs );
    for( std::map<dsstring, DrawSpace::Core::Configurable*>::iterator it = configs.begin(); it != configs.end(); ++it )
    {
        IntermediatePass* pass = dynamic_cast<IntermediatePass*>( it->second );
        if( pass )
        {
            dsstring pass_name;            
            availables_passes_labels.Add( it->first.c_str() );
        }
    }
    return availables_passes_labels;
}

wxArrayString BasicSceneMainFrame::get_fx_list( void )
{
    wxArrayString availables_fx_labels;
    std::map<dsstring, DrawSpace::Core::Configurable*> configs;

    ConfigsBase::GetInstance()->GetConfigsInstancesList( configs );
    for( std::map<dsstring, DrawSpace::Core::Configurable*>::iterator it = configs.begin(); it != configs.end(); ++it )
    {
        Fx* fx = dynamic_cast<Fx*>( it->second );
        if( fx )
        {
            dsstring pass_name;            
            availables_fx_labels.Add( it->first.c_str() );
        }
    }
    return availables_fx_labels;
}

wxArrayString BasicSceneMainFrame::get_textures_list( void )
{
    wxArrayString availables_textures_labels;

    std::map<dsstring, Asset*> assets_list;

    AssetsBase::GetInstance()->GetAssetsList( assets_list );    
    for( std::map<dsstring, Asset*>::iterator it = assets_list.begin(); it != assets_list.end(); ++it )
    {
        if( dynamic_cast<Texture*>( it->second ) )
        {
            availables_textures_labels.Add( it->first.c_str() );
        }
    }
    return availables_textures_labels;
}

wxArrayString BasicSceneMainFrame::get_shaders_list( void )
{
    wxArrayString availables_shaders_labels;

    std::map<dsstring, Asset*> assets_list;

    AssetsBase::GetInstance()->GetAssetsList( assets_list );    
    for( std::map<dsstring, Asset*>::iterator it = assets_list.begin(); it != assets_list.end(); ++it )
    {
        if( dynamic_cast<Shader*>( it->second ) )
        {
            availables_shaders_labels.Add( it->first.c_str() );
        }
    }

    return availables_shaders_labels;
}

wxArrayString BasicSceneMainFrame::get_meshes_list( void )
{
    wxArrayString availables_meshes_labels;

    std::map<dsstring, Asset*> assets_list;

    AssetsBase::GetInstance()->GetAssetsList( assets_list );    
    for( std::map<dsstring, Asset*>::iterator it = assets_list.begin(); it != assets_list.end(); ++it )
    {
        if( dynamic_cast<Meshe*>( it->second ) )
        {
            availables_meshes_labels.Add( it->first.c_str() );
        }
    }

    return availables_meshes_labels;
}

wxArrayString BasicSceneMainFrame::get_fonts_list( void )
{
    wxArrayString availables_fonts_labels;

    std::map<dsstring, Asset*> assets_list;

    AssetsBase::GetInstance()->GetAssetsList( assets_list );    
    for( std::map<dsstring, Asset*>::iterator it = assets_list.begin(); it != assets_list.end(); ++it )
    {
        if( dynamic_cast<Font*>( it->second ) )
        {
            availables_fonts_labels.Add( it->first.c_str() );
        }
    }

    return availables_fonts_labels;
}

wxArrayString BasicSceneMainFrame::insert_void_choice( const wxArrayString& p_array )
{
    wxArrayString completed_array = p_array;
    completed_array.Insert( "...", 0 );

    return completed_array;
}

void BasicSceneMainFrame::on_applybutton_clicked( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID
    DIALOG_PROPERTIES_VARS

    if( "Spacebox node creation" == DIALOG_TITLE )
    {
        DIALOG_GET_STRING_PROPERTY( "scene name", scene_name )

        DIALOG_GET_INT_PROPERTY( "the beast", var_int )

        DIALOG_GET_FLOAT_PROPERTY( "pi", pi )

        DIALOG_GET_ENUM_PROPERTY( "list", sel_text )

        DIALOG_WXSTRING_TO_DSSTRING( scene_name, scene_name2 )
        DIALOG_WXSTRING_TO_DSSTRING( sel_text, sel_text2 )

        
        if( DIALOG_CHECK_PROPERTY( "PASS_0" ) )
        {
            _asm nop
        }
    }
    
}

void BasicSceneMainFrame::on_specificbutton0_clicked( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID

    if( "Spacebox node creation" == DIALOG_TITLE )
    {
        DIALOG_SPECIFIC0_LABEL( "PASS_%d", pass_label )

        DIALOG_APPENDROOT_NODE( pass_label, pass_root )        
        DIALOG_APPENDNODE_BOOL( pass_root, "enable", true );

        DIALOG_BUILD_LABELS( 32, "texture_stage_%d", textures_stages_labels )

        for( size_t i = 0; i < textures_stages_labels.size(); i++ )
        {
            DIALOG_APPENDNODE_NODE( pass_root, textures_stages_labels[i], texture_stage )


            DIALOG_APPENDNODE_ENUM( texture_stage, "source", get_textures_list() )
            DIALOG_APPENDNODE_INTEGER( texture_stage, "order", 200 )
        }
                       
        DIALOG_FINALIZE
    }
}

void BasicSceneMainFrame::on_specificbutton1_clicked( BasicSceneObjectPropertiesDialog* p_dialog )
{
    
}
