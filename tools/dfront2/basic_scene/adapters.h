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

#ifndef _ADAPTERS_H_
#define _ADAPTERS_H_

#include <wx/listctrl.h>
#include <wx/propgrid/propgrid.h>
#include "drawspace.h"
#include "BasicSceneObjectPropertiesDialog.h"
#include "BasicSceneMainFrame.h"

class wxWidgetAdapter
{
protected:

    static wxWidgetAdapter* m_instance;
   
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applypassshadervalues_callback;

    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applylinearmvtvalues_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applycircularmvtvalues_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applyfpsmvtvalues_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applyfreemvtvalues_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applyheadmvtvalues_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applyspectatormvtvalues_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applylonglatmvtvalues_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applycameravalues_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applycameraprops_callback;

    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applytransfonodevalues_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applyspaceboxvalues_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applyspaceboxaddpassslot_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applymatrixstackvalue_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applymatrixstackaddmatrix_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applymatrixstackclearall_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applytransfosourcemodification_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applyregistervalues_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applyregisterprops_callback;
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applymovementcontrolprops_callback;

    long m_pass_slot_index;
    long m_matrix_slot_index;

    


    void on_applypassshadervalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applylinearmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applycircularmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applyfpsmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applyfreemvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applyheadmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applyspectatormvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applylonglatmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applycameravalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applycameraprops( BasicSceneObjectPropertiesDialog* p_dialog );


    void on_applytransfonodevalues( BasicSceneObjectPropertiesDialog* p_dialog );

    void on_applyspaceboxvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applyspaceboxaddpassslot( BasicSceneObjectPropertiesDialog* p_dialog );

    void on_applymatrixstackvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applymatrixstackaddmatrix( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applymatrixstackclearall( BasicSceneObjectPropertiesDialog* p_dialog );

    void on_applytransfosourcemodification( BasicSceneObjectPropertiesDialog* p_dialog );

    void on_applyregistervalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applyregisterprops( BasicSceneObjectPropertiesDialog* p_dialog );

    void on_applymovementcontrolprops( BasicSceneObjectPropertiesDialog* p_dialog );
    
    

    wxWidgetAdapter( void );


public:

    long m_register_index;

    ~wxWidgetAdapter( void );

    static wxWidgetAdapter* GetInstance( void )
    {
        if( !m_instance )
        {
            m_instance = new wxWidgetAdapter;
        }
        return m_instance;
    }

    void AdaptAssetsList( wxListCtrl* p_listctrl );
    void AdaptConfigsList( wxListCtrl* p_listctrl );
    void AdaptPassesList( wxListCtrl* p_listctrl );
    void AdaptPassesShaderParamsList( DrawSpace::Pass* p_pass, wxListCtrl* p_listctrl );
    void AdaptMvtsList( std::map<dsstring, BasicSceneMainFrame::MovementEntry>* p_map, wxListCtrl* p_listctrl );
    void AdaptCamerasList( DrawSpace::Scenegraph* p_scenegraph, wxListCtrl* p_listctrl );
    
    void AdaptRegistersList( std::map<dsstring, BasicSceneMainFrame::RegisterEntry>* p_registers, wxListCtrl* p_listctrl );

    void AdaptFxProps( DrawSpace::Core::Fx* p_fx, wxPropertyGrid* p_propertygrid );
    void AdaptPassProps( bool p_intermediate_pass, DrawSpace::Pass* p_pass, wxPropertyGrid* p_propertygrid );
    void AdaptCameraProps( DrawSpace::Dynamics::CameraPoint* p_camera, BasicSceneObjectPropertiesDialog* p_dialog );

    void AdaptPassShaderValuesPropsModification( DrawSpace::Pass* p_pass, char* p_param_id, BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptCameraPropsModification( DrawSpace::Dynamics::CameraPoint* p_camera, BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptRegisterPropsModification( BasicSceneMainFrame::RegisterEntry* p_regentry, BasicSceneObjectPropertiesDialog* p_dialog );
    
    void AdaptLinearMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptCircularMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptFPSMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptFreeMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptHeadMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptSpectatorMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptLongLatMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptCameraCreationProps( std::map<dsstring, BasicSceneMainFrame::MovementEntry>* p_mvts_map, BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptSpaceBoxCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptRegisterCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );

    void AdaptLinearMvtProps( const dsstring& p_mvtname, DrawSpace::Core::LinearMovement* p_movement, BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptCircularMvtProps( const dsstring& p_mvtname, DrawSpace::Core::CircularMovement* p_movement, BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptFpsMvtProps( const dsstring& p_mvtname, DrawSpace::Core::FPSMovement* p_movement, BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptFreeMvtProps( const dsstring& p_mvtname, DrawSpace::Core::FreeMovement* p_movement, BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptHeadMvtProps( const dsstring& p_mvtname, DrawSpace::Core::HeadMovement* p_movement, BasicSceneObjectPropertiesDialog* p_dialog );
    
    void AdaptTransfonodeCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );


    void AdaptSpectatorMvtProps( const dsstring& p_mvtname, DrawSpace::Core::SpectatorMovement* p_movement, BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptLongLatMvtProps( const dsstring& p_mvtname, DrawSpace::Core::LongLatMovement* p_movement, BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptRegProps( const dsstring& p_alias, BasicSceneMainFrame::RegisterEntry* p_reg, BasicSceneObjectPropertiesDialog* p_dialog );
    
    void AdaptTransfoSourceModification( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptMatrixStackEdition( std::map<dsstring, BasicSceneMainFrame::RegisterEntry>* p_registers, BasicSceneMainFrame::TransformationNodeEntry* p_transfonode, BasicSceneObjectPropertiesDialog* p_dialog );

    void AdaptMovementControlProps( const dsstring& p_mvtalias, std::map<dsstring, BasicSceneMainFrame::RegisterEntry>* p_registers, BasicSceneObjectPropertiesDialog* p_dialog );
    

    void AdaptCameraListComboBox( DrawSpace::Scenegraph* p_scenegraph, wxComboBox* p_combobox );
    void AdaptKeyboardOutputComboBox( std::map<dsstring, BasicSceneMainFrame::MovementEntry>* p_mvts, wxComboBox* p_combobox );

    void AdaptRegistersLastValue( std::map<dsstring, BasicSceneMainFrame::RegisterEntry>* p_registers, wxListCtrl* p_listctrl );

    void InitializeRegister( BasicSceneMainFrame::RegisterEntry* p_reg_entry );

};


#endif
