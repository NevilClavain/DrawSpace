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

#include "adapters.h"

using namespace DrawSpace;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Core;

wxWidgetAdapter* wxWidgetAdapter::m_instance = NULL;

wxWidgetAdapter::wxWidgetAdapter( void )
{
    m_applypassshadervalues_callback = new CallBack<wxWidgetAdapter, void, wxPropertyGrid*>( this, &wxWidgetAdapter::on_applypassshadervalues );
}

wxWidgetAdapter::~wxWidgetAdapter( void )
{

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

    for( std::map<dsstring, Asset*>::iterator it = assets_list.begin();
        it != assets_list.end(); ++it )
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

        id++;
            
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
    for( std::map<dsstring, DrawSpace::Core::Configurable*>::iterator it = instances_list.begin(); it != instances_list.end(); ++it )
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
        
        id++;
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

    wxListItem col3;
    col3.SetId( 3 );
    col3.SetText( "Value" );
    col3.SetWidth( 150 );
    p_listctrl->InsertColumn( 3, col3 );

    ///

    Configurable::PropertiesMap props;
    p_pass->GetPropertiesMap( props );

    
    bool viewportquad = props["viewportquad"].GetPropValue<bool>();

    if( viewportquad )
    {
        long id = 0;
        std::map<dsstring, RenderingNode::ShadersParams> viewportquad_shaderparams = props["viewportquad_shaderparams"].GetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>();
        for( std::map<dsstring, RenderingNode::ShadersParams>::iterator it = viewportquad_shaderparams.begin(); it != viewportquad_shaderparams.end(); ++ it )
        {
            wxListItem item;
            item.SetId( id );
            item.SetText( it->first.c_str() );
            p_listctrl->InsertItem( item );

            char shader_index[32];
            sprintf( shader_index, "%d", it->second.shader_index );

            char param_register[32];
            sprintf( param_register, "%d", it->second.param_register );

            char param_values[64];
            sprintf( param_values, "%.2f %.2f %.2f %.2f", it->second.param_values[0], it->second.param_values[1], it->second.param_values[2], it->second.param_values[3] );

            p_listctrl->SetItem( id, 1, shader_index );
            p_listctrl->SetItem( id, 2, param_register );
            p_listctrl->SetItem( id, 3, param_values );

            p_listctrl->SetItemData( id, (long)p_pass );

            id++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void wxWidgetAdapter::AdaptTextureProps( DrawSpace::Core::Texture* p_texture, wxPropertyGrid* p_propertygrid )
{
    Asset::PropertiesMap props;
    p_texture->GetPropertiesMap( props );

    dsstring assetname = props["assetname"].GetPropValue<dsstring>();
    dsstring filepath = props["filepath"].GetPropValue<dsstring>();
    bool rendertarget = props["rendertarget"].GetPropValue<bool>();
    unsigned long rendetarget_w = props["rendertarget_size"].GetPropValue<unsigned long>( "width" );
    unsigned long rendetarget_h = props["rendertarget_size"].GetPropValue<unsigned long>( "height" );


    p_propertygrid->Append( new wxStringProperty( "assetname", wxPG_LABEL, assetname.c_str() ) );
    //p_propertygrid->DisableProperty( "assetname" );

    p_propertygrid->Append( new wxStringProperty( "filepath", wxPG_LABEL, filepath.c_str() ) );
    //p_propertygrid->DisableProperty( "filepath" );

    p_propertygrid->Append( new wxBoolProperty( "rendertarget", wxPG_LABEL, rendertarget ) );
    //p_propertygrid->DisableProperty( "rendertarget" );

    if( rendertarget )
    {
        p_propertygrid->Append( new wxIntProperty( "rendertarget_size/width", wxPG_LABEL, rendetarget_w ) );
        //p_propertygrid->DisableProperty( "rendertarget_size/width" );
        p_propertygrid->Append( new wxIntProperty( "rendertarget_size/height", wxPG_LABEL, rendetarget_h ) );
        //p_propertygrid->DisableProperty( "rendertarget_size/height" );
    }
    
}

void wxWidgetAdapter::AdaptShaderProps( DrawSpace::Core::Shader* p_shader, wxPropertyGrid* p_propertygrid )
{
    Asset::PropertiesMap props;
    p_shader->GetPropertiesMap( props );

    dsstring assetname = props["assetname"].GetPropValue<dsstring>();
    dsstring filepath = props["filepath"].GetPropValue<dsstring>();
    bool compiled = props["compiled"].GetPropValue<bool>();

    p_propertygrid->Append( new wxStringProperty( "assetname", wxPG_LABEL, assetname.c_str() ) );
    p_propertygrid->Append( new wxStringProperty( "filepath", wxPG_LABEL, filepath.c_str() ) );
    p_propertygrid->Append( new wxBoolProperty( "compiled", wxPG_LABEL, compiled ) );
}

void wxWidgetAdapter::AdaptFontProps( DrawSpace::Core::Font* p_font, wxPropertyGrid* p_propertygrid )
{
    Asset::PropertiesMap props;
    p_font->GetPropertiesMap( props );

    dsstring assetname = props["assetname"].GetPropValue<dsstring>();
    dsstring plugin = props["plugin"].GetPropValue<dsstring>();
    dsstring texturefilepath = props["filespath"].GetPropValue<dsstring>( "texturefilepath" );
    dsstring metricsfilepath = props["filespath"].GetPropValue<dsstring>( "metricsfilepath" );

    p_propertygrid->Append( new wxStringProperty( "assetname", wxPG_LABEL, assetname.c_str() ) );
    p_propertygrid->Append( new wxStringProperty( "texturefilepath", wxPG_LABEL, texturefilepath.c_str() ) );
    p_propertygrid->Append( new wxStringProperty( "metricsfilepath", wxPG_LABEL, metricsfilepath.c_str() ) );
    p_propertygrid->Append( new wxStringProperty( "plugin", wxPG_LABEL, plugin.c_str() ) );   
}

void wxWidgetAdapter::AdaptMesheProps( DrawSpace::Core::Meshe* p_meshe, wxPropertyGrid* p_propertygrid )
{
    Asset::PropertiesMap props;
    p_meshe->GetPropertiesMap( props );

    dsstring assetname = props["assetname"].GetPropValue<dsstring>();
    dsstring filepath = props["filepath"].GetPropValue<dsstring>();
    long index = props["index"].GetPropValue<long>();
    dsstring plugin = props["plugin"].GetPropValue<dsstring>();

    p_propertygrid->Append( new wxStringProperty( "assetname", wxPG_LABEL, assetname.c_str() ) );
    p_propertygrid->Append( new wxStringProperty( "filepath", wxPG_LABEL, filepath.c_str() ) );
    p_propertygrid->Append( new wxIntProperty( "index", wxPG_LABEL, index ) );
    p_propertygrid->Append( new wxStringProperty( "plugin", wxPG_LABEL, plugin.c_str() ) );      
}

void wxWidgetAdapter::AdaptFxProps( DrawSpace::Core::Fx* p_fx, wxPropertyGrid* p_propertygrid )
{
    Configurable::PropertiesMap props;
    p_fx->GetPropertiesMap( props );

    std::vector<dsstring> shaders_list = props["shaders"].GetPropValue<std::vector<dsstring>>();
    for( size_t i = 0; i < shaders_list.size(); i++ )
    {
        char shader_index[32];
        sprintf( shader_index, "shader/%d", i );
        dsstring shader_name = shaders_list[i];
        p_propertygrid->Append( new wxStringProperty( shader_index, wxPG_LABEL, shader_name.c_str() ) );
    }

    std::vector<RenderState> rsin_list = props["renderstates_in"].GetPropValue<std::vector<RenderState>>();
    for( size_t i = 0; i < rsin_list.size(); i++ )
    {
        dsstring renderstate_op;
        dsstring renderstate_arg;

        RenderState rs = rsin_list[i];
        
        char rsin_index[32];
        sprintf( rsin_index, "renderstate_in/%d", i );
        
        rs.GetOperationToString( renderstate_op );
        rs.GetArg( renderstate_arg );
        dsstring final = renderstate_op + "," + renderstate_arg;
        p_propertygrid->Append( new wxStringProperty( rsin_index, wxPG_LABEL, final.c_str() ) );
    }

    std::vector<RenderState> rsout_list = props["renderstates_out"].GetPropValue<std::vector<RenderState>>();
    for( size_t i = 0; i < rsout_list.size(); i++ )
    {
        dsstring renderstate_op;
        dsstring renderstate_arg;

        RenderState rs = rsout_list[i];
        
        char rsout_index[32];
        sprintf( rsout_index, "renderstate_out:%d", i );
        
        rs.GetOperationToString( renderstate_op );
        rs.GetArg( renderstate_arg );
        dsstring final = renderstate_op + "," + renderstate_arg;
        p_propertygrid->Append( new wxStringProperty( rsout_index, wxPG_LABEL, final.c_str() ) );
    }
}

void wxWidgetAdapter::AdaptPassProps( bool p_intermediate_pass, DrawSpace::Pass* p_pass, wxPropertyGrid* p_propertygrid )
{
    Configurable::PropertiesMap props;
    p_pass->GetPropertiesMap( props );

    dsstring passname = props["passname"].GetPropValue<dsstring>();
    p_propertygrid->Append( new wxStringProperty( "passname", wxPG_LABEL, passname.c_str() ) );

    if( p_intermediate_pass )
    {
        bool targetdimsfromrenderer = props["targetdimsfromrenderer"].GetPropValue<bool>();
        long targetdims_width = props["targetdims"].GetPropValue<long>( "width" );
        long targetdims_height = props["targetdims"].GetPropValue<long>( "height" );

        p_propertygrid->Append( new wxBoolProperty( "targetdimsfromrenderer", wxPG_LABEL, targetdimsfromrenderer ) );

        if( !targetdimsfromrenderer )
        {
            p_propertygrid->Append( new wxIntProperty( "targetdims/width", wxPG_LABEL, targetdims_width ) );
            p_propertygrid->Append( new wxIntProperty( "targetdims/height", wxPG_LABEL, targetdims_height ) );
        }
    }

    p_propertygrid->Append( new wxBoolProperty( "enabledepthclear", wxPG_LABEL, props["enabledepthclear"].GetPropValue<bool>() ) );
    p_propertygrid->Append( new wxBoolProperty( "enabletargetclear", wxPG_LABEL, props["enabletargetclear"].GetPropValue<bool>() ) );

    p_propertygrid->Append( new wxIntProperty( "targetclearcolor/r", wxPG_LABEL, props["targetclearcolor"].GetPropValue<unsigned char>( "r" ) ) );
    p_propertygrid->Append( new wxIntProperty( "targetclearcolor/g", wxPG_LABEL, props["targetclearcolor"].GetPropValue<unsigned char>( "g" ) ) );
    p_propertygrid->Append( new wxIntProperty( "targetclearcolor/b", wxPG_LABEL, props["targetclearcolor"].GetPropValue<unsigned char>( "b" ) ) );

    bool viewportquad = props["viewportquad"].GetPropValue<bool>();

    p_propertygrid->Append( new wxBoolProperty( "viewportquad", wxPG_LABEL, viewportquad ) );

    if( viewportquad )
    {
        p_propertygrid->Append( new wxStringProperty( "viewportquad_fx", wxPG_LABEL, props["viewportquad_fx"].GetPropValue<dsstring>() ) );
        
        std::vector<std::pair<long, Pass::TextureSourceName>> viewportquad_textures;
        viewportquad_textures = props["viewportquad_textures"].GetPropValue<std::vector<std::pair<long, Pass::TextureSourceName>>>();
        for( size_t i = 0; i < viewportquad_textures.size(); i++ )
        {
            Pass::TextureSourceName texture_source_name = viewportquad_textures[i].second;
            long stage = viewportquad_textures[i].first;

            char texture_entry[128];

            sprintf( texture_entry, "textures/%d, stage %d", i, stage );

            dsstring final;
            if( Pass::PASS_NAME == texture_source_name.source )
            {
                final = "Pass : " + texture_source_name.name;
            }
            else
            {
                final = "Texture : " + texture_source_name.name;
            }

            p_propertygrid->Append( new wxStringProperty( texture_entry, wxPG_LABEL, final.c_str() ) );
        }

        std::map<dsstring, RenderingNode::ShadersParams> viewportquad_shaderparams = props["viewportquad_shaderparams"].GetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>();
        for( std::map<dsstring, RenderingNode::ShadersParams>::iterator it = viewportquad_shaderparams.begin(); it != viewportquad_shaderparams.end(); ++ it )
        {
            char param_entry[128];
            sprintf( param_entry, "shaderparams/%s, shader %d register %d", it->first.c_str(), it->second.shader_index, it->second.param_register );

            char param_value[64];

            sprintf( param_value, "%.2f %.2f %.2f %.2f", it->second.param_values[0], it->second.param_values[1], it->second.param_values[2], it->second.param_values[3] );

            p_propertygrid->Append( new wxStringProperty( param_entry, wxPG_LABEL, param_value ) );
        }
    }
}

void wxWidgetAdapter::AdaptPassShaderValuesProps( DrawSpace::Pass* p_pass, char* p_param_id, BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    Configurable::PropertiesMap props;
    p_pass->GetPropertiesMap( props );

    std::map<dsstring, RenderingNode::ShadersParams> viewportquad_shaderparams = props["viewportquad_shaderparams"].GetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>();

    dsreal x = viewportquad_shaderparams[p_param_id].param_values[0];
    dsreal y = viewportquad_shaderparams[p_param_id].param_values[1];
    dsreal z = viewportquad_shaderparams[p_param_id].param_values[2];
    dsreal w = viewportquad_shaderparams[p_param_id].param_values[3];

    propertygrid->Append( new wxFloatProperty( "x", wxPG_LABEL, x ) );
    propertygrid->Append( new wxFloatProperty( "y", wxPG_LABEL, y ) );
    propertygrid->Append( new wxFloatProperty( "z", wxPG_LABEL, z ) );
    propertygrid->Append( new wxFloatProperty( "w", wxPG_LABEL, w ) );

    p_dialog->RegisterApplyButtonHandler( m_applypassshadervalues_callback );
}

void wxWidgetAdapter::on_applypassshadervalues( wxPropertyGrid* p_propertygrid )
{
    wxFloatProperty* prop;
    wxAny value;
    dsreal x, y, z, w;
    
    prop = static_cast<wxFloatProperty*>( p_propertygrid->GetProperty( "x" ) );
    value = prop->GetValue();   
    value.GetAs<double>( &x );

    prop = static_cast<wxFloatProperty*>( p_propertygrid->GetProperty( "y" ) );
    value = prop->GetValue();   
    value.GetAs<double>( &y );

    prop = static_cast<wxFloatProperty*>( p_propertygrid->GetProperty( "z" ) );
    value = prop->GetValue();   
    value.GetAs<double>( &z );

    prop = static_cast<wxFloatProperty*>( p_propertygrid->GetProperty( "w" ) );
    value = prop->GetValue();   
    value.GetAs<double>( &w );
}