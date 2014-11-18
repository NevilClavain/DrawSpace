
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


    void on_applypassshadervalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applylinearmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applycircularmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applyfpsmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applyfreemvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applyheadmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applyspectatormvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_applylonglatmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog );

    wxWidgetAdapter( void );


public:

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
    void AdaptMvtsList( std::map<dsstring, DrawSpace::Core::Movement*>* p_map, wxListCtrl* p_listctrl );


    void AdaptTextureProps( DrawSpace::Core::Texture* p_texture, wxPropertyGrid* p_propertygrid );
    void AdaptShaderProps( DrawSpace::Core::Shader* p_shader, wxPropertyGrid* p_propertygrid );
    void AdaptFontProps( DrawSpace::Core::Font* p_font, wxPropertyGrid* p_propertygrid );
    void AdaptMesheProps( DrawSpace::Core::Meshe* p_meshe, wxPropertyGrid* p_propertygrid );
    void AdaptFxProps( DrawSpace::Core::Fx* p_fx, wxPropertyGrid* p_propertygrid );
    void AdaptPassProps( bool p_intermediate_pass, DrawSpace::Pass* p_pass, wxPropertyGrid* p_propertygrid );
    void AdaptPassShaderValuesProps( DrawSpace::Pass* p_pass, char* p_param_id, BasicSceneObjectPropertiesDialog* p_dialog );

    void AdaptLinearMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptCircularMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptFPSMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptFreeMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptHeadMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptSpectatorMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptLongLatMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog );

    void AdaptLinearMvtProps( const dsstring& p_mvtname, DrawSpace::Core::LinearMovement* p_movement, BasicSceneObjectPropertiesDialog* p_dialog );
    void AdaptCircularMvtProps( const dsstring& p_mvtname, DrawSpace::Core::CircularMovement* p_movement, BasicSceneObjectPropertiesDialog* p_dialog );

};


#endif