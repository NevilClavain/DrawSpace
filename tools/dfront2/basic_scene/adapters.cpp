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
#include "adapters.h"
#include "buildobjects.h"

using namespace DrawSpace;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

wxWidgetAdapter* wxWidgetAdapter::m_instance = NULL;

wxWidgetAdapter::wxWidgetAdapter( void )
{
    m_applypassshadervalues_callback = new CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>( this, &wxWidgetAdapter::on_applypassshadervalues );

    m_register_index = 0;
}

wxWidgetAdapter::~wxWidgetAdapter( void )
{
    delete m_applypassshadervalues_callback;
    
   
    
}

void wxWidgetAdapter::AdaptAssetsList( wxListCtrl* p_listctrl )
{
    // columns creations   
    wxListItem col0;
    col0.SetId( 0 );
    col0.SetText( "Name" );
    col0.SetWidth( 150 );
    p_listctrl->InsertColumn( 0, col0 );

    wxListItem col1;
    col1.SetId( 1 );
    col1.SetText( "Type" );
    col1.SetWidth( 150 );
    p_listctrl->InsertColumn( 1, col1 );

    ///

    std::map<dsstring, Asset*> assets_list;
    AssetsBase::GetInstance()->GetAssetsList( assets_list );

    long id = 0;

    for( std::map<dsstring, Asset*>::iterator it = assets_list.begin(); it != assets_list.end(); ++it, id++ )
    {

        wxListItem item;
        item.SetId( id );

        dsstring assetname;
        it->second->GetName( assetname );

        item.SetText( assetname.c_str() );

        p_listctrl->InsertItem( item );

        dsstring type_name;

        if( dynamic_cast<Texture*>( it->second ) )
        {
            type_name = TEXTURE_TEXT_KEYWORD;
        }
        else if( dynamic_cast<Font*>( it->second ) )
        {
            type_name = FONT_TEXT_KEYWORD;
        }
        else if( dynamic_cast<Meshe*>( it->second ) )
        {
            type_name = MESHE_TEXT_KEYWORD;
        }
        else if( dynamic_cast<Shader*>( it->second ) )
        {
            type_name = SHADER_TEXT_KEYWORD;
        }
        else
        {
            type_name = "???";
        }
        p_listctrl->SetItem( id, 1, type_name.c_str() );

        p_listctrl->SetItemData( id, (long)it->second );            
    }   
}

void wxWidgetAdapter::AdaptConfigsList( wxListCtrl* p_listctrl )
{
    // columns creations   
    wxListItem col0;
    col0.SetId( 0 );
    col0.SetText( "Name" );
    col0.SetWidth( 110 );
    p_listctrl->InsertColumn( 0, col0 );

    wxListItem col1;
    col1.SetId( 1 );
    col1.SetText( "Type" );
    col1.SetWidth( 100 );
    p_listctrl->InsertColumn( 1, col1 );

    ///

    std::map<dsstring, DrawSpace::Core::Configurable*> instances_list;

    ConfigsBase::GetInstance()->GetConfigsInstancesList( instances_list );


    long id = 0;
    for( std::map<dsstring, DrawSpace::Core::Configurable*>::iterator it = instances_list.begin(); it != instances_list.end(); ++it, id++ )
    {
        wxListItem item;
        item.SetId( id );

        dsstring configname = it->first;
        item.SetText( configname.c_str() );
        p_listctrl->InsertItem( item );


        dsstring type_name;

        if( dynamic_cast<Fx*>( it->second ) )
        {
            type_name = FX_TEXT_KEYWORD;
        }
        else if( dynamic_cast<IntermediatePass*>( it->second ) )
        {
            type_name = INTERMEDIATEPASS_TEXT_KEYWORD;
        }
        else if( dynamic_cast<FinalPass*>( it->second ) )
        {
            type_name = FINALPASS_TEXT_KEYWORD;
        }
        else
        {
            type_name = "???";
        }

        p_listctrl->SetItem( id, 1, type_name.c_str() );
        p_listctrl->SetItemData( id, (long)it->second );       
    }
}

void wxWidgetAdapter::AdaptPassesList( wxListCtrl* p_listctrl )
{
    // columns creations   
    wxListItem col0;
    col0.SetId( 0 );
    col0.SetText( "Pass Name" );
    col0.SetWidth( 110 );
    p_listctrl->InsertColumn( 0, col0 );

    wxListItem col1;
    col1.SetId( 1 );
    col1.SetText( "Pass Type" );
    col1.SetWidth( 100 );
    p_listctrl->InsertColumn( 1, col1 );

    ///

    std::map<dsstring, DrawSpace::Core::Configurable*> instances_list;

    ConfigsBase::GetInstance()->GetConfigsInstancesList( instances_list );


    long id = 0;
    for( std::map<dsstring, DrawSpace::Core::Configurable*>::iterator it = instances_list.begin(); it != instances_list.end(); ++it )
    {
        dsstring configname = it->first;                
        bool insert = true;

        dsstring type_name;

        if( dynamic_cast<IntermediatePass*>( it->second ) )
        {
            type_name = INTERMEDIATEPASS_TEXT_KEYWORD;
        }
        else if( dynamic_cast<FinalPass*>( it->second ) )
        {
            type_name = FINALPASS_TEXT_KEYWORD;
        }
        else
        {
            insert = false;
        }

        if( insert )
        {

            wxListItem item;
            item.SetId( id );

            item.SetText( configname.c_str() );
            p_listctrl->InsertItem( item );
            p_listctrl->SetItem( id, 1, type_name.c_str() );
            p_listctrl->SetItemData( id, (long)it->second );

            id++;
        }                
    }
}

void wxWidgetAdapter::AdaptPassesShaderParamsList( DrawSpace::Pass* p_pass, wxListCtrl* p_listctrl )
{
    p_listctrl->ClearAll();

    wxListItem col0;
    col0.SetId( 0 );
    col0.SetText( "Parameter Name" );
    col0.SetWidth( 110 );
    p_listctrl->InsertColumn( 0, col0 );

    wxListItem col1;
    col1.SetId( 1 );
    col1.SetText( "Shader index" );
    col1.SetWidth( 100 );
    p_listctrl->InsertColumn( 1, col1 );

    wxListItem col2;
    col2.SetId( 2 );
    col2.SetText( "Register" );
    col2.SetWidth( 100 );
    p_listctrl->InsertColumn( 2, col2 );

    /*
    wxListItem col3;
    col3.SetId( 3 );
    col3.SetText( "Value" );
    col3.SetWidth( 150 );
    p_listctrl->InsertColumn( 3, col3 );
    */
    ///

    Configurable::PropertiesMap props;
    p_pass->GetPropertiesMap( props );

    
    bool viewportquad = props["viewportquad"].GetPropValue<bool>();

    if( viewportquad )
    {
        long id = 0;
        std::map<dsstring, RenderingNode::ShadersParams> viewportquad_shaderparams = props["viewportquad_shaderparams"].GetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>();
        for( std::map<dsstring, RenderingNode::ShadersParams>::iterator it = viewportquad_shaderparams.begin(); it != viewportquad_shaderparams.end(); ++it, id++ )
        {
            wxListItem item;
            item.SetId( id );
            item.SetText( it->first.c_str() );
            p_listctrl->InsertItem( item );

            char shader_index[32];
            sprintf( shader_index, "%d", it->second.shader_index );

            char param_register[32];
            sprintf( param_register, "%d", it->second.param_register );

            //char param_values[64];
            //sprintf( param_values, "%f %f %f %f", it->second.param_values[0], it->second.param_values[1], it->second.param_values[2], it->second.param_values[3] );

            p_listctrl->SetItem( id, 1, shader_index );
            p_listctrl->SetItem( id, 2, param_register );
            //p_listctrl->SetItem( id, 3, param_values );

            p_listctrl->SetItemData( id, (long)p_pass );
        }
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////


void wxWidgetAdapter::AdaptPassShaderValuesPropsModification( Pass* p_pass, char* p_param_id, BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    Configurable::PropertiesMap props;
    p_pass->GetPropertiesMap( props );

    std::map<dsstring, RenderingNode::ShadersParams> viewportquad_shaderparams = props["viewportquad_shaderparams"].GetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>();

    dsreal x = viewportquad_shaderparams[p_param_id].param_values[0];
    dsreal y = viewportquad_shaderparams[p_param_id].param_values[1];
    dsreal z = viewportquad_shaderparams[p_param_id].param_values[2];
    dsreal w = viewportquad_shaderparams[p_param_id].param_values[3];

    propertygrid->Append( new wxStringProperty( "param id", wxPG_LABEL, p_param_id ) );
    propertygrid->Append( new wxFloatProperty( "x", wxPG_LABEL, x ) );
    propertygrid->Append( new wxFloatProperty( "y", wxPG_LABEL, y ) );
    propertygrid->Append( new wxFloatProperty( "z", wxPG_LABEL, z ) );
    propertygrid->Append( new wxFloatProperty( "w", wxPG_LABEL, w ) );

    char* param_id = new char[strlen( p_param_id ) + 1];
    strcpy( param_id, p_param_id );

    p_dialog->RegisterApplyButtonHandler( m_applypassshadervalues_callback );
    p_dialog->SetData( "pass", p_pass );

    p_dialog->SetData( "param_id", param_id );
}

void wxWidgetAdapter::on_applypassshadervalues( BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    wxFloatProperty* prop;
    wxAny value;
    dsreal x, y, z, w;
    
    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "x" ) );
    value = prop->GetValue();   
    value.GetAs<double>( &x );

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "y" ) );
    value = prop->GetValue();   
    value.GetAs<double>( &y );

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "z" ) );
    value = prop->GetValue();   
    value.GetAs<double>( &z );

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "w" ) );
    value = prop->GetValue();   
    value.GetAs<double>( &w );

    Pass* pass = (Pass*)p_dialog->GetData( "pass" );
    char* param_id = (char*)p_dialog->GetData( "param_id" );

    // apply pass viewportquad shader values modifications
    pass->GetViewportQuad()->SetShaderRealVector( param_id, Vector( x, y, z, w ) );

    // update pass properties
    Configurable::PropertiesMap props;
    pass->GetPropertiesMap( props );

    std::map<dsstring, RenderingNode::ShadersParams> viewportquad_shaderparams = props["viewportquad_shaderparams"].GetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>();

    viewportquad_shaderparams[param_id].param_values[0] = x;
    viewportquad_shaderparams[param_id].param_values[1] = y;
    viewportquad_shaderparams[param_id].param_values[2] = z;
    viewportquad_shaderparams[param_id].param_values[3] = w;

    props["viewportquad_shaderparams"].SetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>( viewportquad_shaderparams );

    pass->SetPropertiesMap( props );

    // update mainframe list ctrl
    wxListCtrl* ctrl = (wxListCtrl*)p_dialog->GetData( "ctrl" );
    AdaptPassesShaderParamsList( pass, ctrl );

    
    // release string allocated in wxWidgetAdapter::AdaptPassShaderValuesPropsModification()
    //delete[] param_id;
    //p_dialog->Close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
