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

#include <wx/wx.h>
#include "adapters.h"

using namespace DrawSpace;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

wxWidgetAdapter* wxWidgetAdapter::m_instance = NULL;

wxWidgetAdapter::wxWidgetAdapter( void )
{
    m_applypassshadervalues_callback = new CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>( this, &wxWidgetAdapter::on_applypassshadervalues );

    m_applylinearmvtvalues_callback = new CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>( this, &wxWidgetAdapter::on_applylinearmvtvalues );
    m_applycircularmvtvalues_callback = new CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>( this, &wxWidgetAdapter::on_applycircularmvtvalues );
    m_applyfpsmvtvalues_callback = new CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>( this, &wxWidgetAdapter::on_applyfpsmvtvalues );
    m_applyfreemvtvalues_callback = new CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>( this, &wxWidgetAdapter::on_applyfreemvtvalues );
    m_applyheadmvtvalues_callback = new CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>( this, &wxWidgetAdapter::on_applyheadmvtvalues );
    m_applyspectatormvtvalues_callback = new CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>( this, &wxWidgetAdapter::on_applyspectatormvtvalues );
    m_applylonglatmvtvalues_callback = new CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>( this, &wxWidgetAdapter::on_applylonglatmvtvalues );
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


void wxWidgetAdapter::AdaptMvtsList( std::map<dsstring, DrawSpace::Core::Movement*>* p_map, wxListCtrl* p_listctrl )
{
    p_listctrl->ClearAll();

    wxListItem col0;
    col0.SetId( 0 );
    col0.SetText( "Movement alias" );
    col0.SetWidth( 110 );
    p_listctrl->InsertColumn( 0, col0 );

    wxListItem col1;
    col1.SetId( 1 );
    col1.SetText( "Movement type" );
    col1.SetWidth( 100 );
    p_listctrl->InsertColumn( 1, col1 );

    long id = 0;
    for( std::map<dsstring, Movement*>::iterator it = p_map->begin(); it != p_map->end(); ++it )
    {
        Movement* mvt = it->second;

        dsstring alias = it->first;

        wxListItem item;
        item.SetId( id );
        item.SetText( alias.c_str() );
        p_listctrl->InsertItem( item );


        dsstring type_name;

        if( dynamic_cast<LinearMovement*>( mvt ) )
        {
            type_name = "Linear";
        }
        else if( dynamic_cast<CircularMovement*>( mvt ) )
        {
            type_name = "Circular";
        }
        else if( dynamic_cast<FPSMovement*>( mvt ) )
        {
            type_name = "FPS";
        }
        else if( dynamic_cast<FreeMovement*>( mvt ) )
        {
            type_name = "Free";
        }
        else if( dynamic_cast<HeadMovement*>( mvt ) )
        {
            type_name = "Head";
        }
        else if( dynamic_cast<SpectatorMovement*>( mvt ) )
        {
            type_name = "Spectator";
        }
        else if( dynamic_cast<LongLatMovement*>( mvt ) )
        {
            type_name = "Longlat";
        }
        p_listctrl->SetItem( id, 1, type_name.c_str() );

        p_listctrl->SetItemData( id, (long)mvt );

        id++;
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

    if( shaders_list.size() > 0 )
    {
        wxPGProperty* shaderslist_prop = p_propertygrid->Append( new wxStringProperty( "shaders", wxPG_LABEL, "<composed>" ) );

        for( size_t i = 0; i < shaders_list.size(); i++ )
        {
            char shader_index[32];
            sprintf( shader_index, "%d", i );

            p_propertygrid->AppendIn( shaderslist_prop, new wxStringProperty( shader_index, wxPG_LABEL, shaders_list[i].c_str() ) );
        }
    }

    std::vector<RenderState> rsin_list = props["renderstates_in"].GetPropValue<std::vector<RenderState>>();

    if( rsin_list.size() > 0 )
    {
        wxPGProperty* rsinlist_prop = p_propertygrid->Append( new wxStringProperty( "renderstates_in", wxPG_LABEL, "<composed>" ) );
        for( size_t i = 0; i < rsin_list.size(); i++ )
        {
            dsstring renderstate_op;
            dsstring renderstate_arg;

            RenderState rs = rsin_list[i];
            
            char rsin_index[32];
            
            sprintf( rsin_index, "%d", i );

            wxPGProperty* rsinlistindex_prop = p_propertygrid->AppendIn( rsinlist_prop, new wxStringProperty( rsin_index, wxPG_LABEL, "<composed>" ) );

            
            rs.GetOperationToString( renderstate_op );
            rs.GetArg( renderstate_arg );

            p_propertygrid->AppendIn( rsinlistindex_prop, new wxStringProperty( "Operation", wxPG_LABEL, renderstate_op.c_str() ) );
            p_propertygrid->AppendIn( rsinlistindex_prop, new wxStringProperty( "Argument", wxPG_LABEL, renderstate_arg.c_str() ) );
        }
    }

    std::vector<RenderState> rsout_list = props["renderstates_out"].GetPropValue<std::vector<RenderState>>();
    if( rsout_list.size() > 0 )
    {
        wxPGProperty* rsoutlist_prop = p_propertygrid->Append( new wxStringProperty( "renderstates_out", wxPG_LABEL, "<composed>" ) );
        for( size_t i = 0; i < rsout_list.size(); i++ )
        {
            dsstring renderstate_op;
            dsstring renderstate_arg;

            RenderState rs = rsout_list[i];
            
            char rsout_index[32];

            sprintf( rsout_index, "%d", i );

            wxPGProperty* rsoutlistindex_prop = p_propertygrid->AppendIn( rsoutlist_prop, new wxStringProperty( rsout_index, wxPG_LABEL, "<composed>" ) );
            
            rs.GetOperationToString( renderstate_op );
            rs.GetArg( renderstate_arg );

            p_propertygrid->AppendIn( rsoutlistindex_prop, new wxStringProperty( "Operation", wxPG_LABEL, renderstate_op.c_str() ) );
            p_propertygrid->AppendIn( rsoutlistindex_prop, new wxStringProperty( "Argument", wxPG_LABEL, renderstate_arg.c_str() ) );

        }
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
            wxPGProperty* targetdims_prop = p_propertygrid->Append( new wxStringProperty( "targetdims", wxPG_LABEL, "<composed>" ) );
            p_propertygrid->AppendIn( targetdims_prop, new wxIntProperty( "width", wxPG_LABEL, targetdims_width ) );
            p_propertygrid->AppendIn( targetdims_prop, new wxIntProperty( "height", wxPG_LABEL, targetdims_height ) );
        }
    }

    p_propertygrid->Append( new wxBoolProperty( "enabledepthclear", wxPG_LABEL, props["enabledepthclear"].GetPropValue<bool>() ) );
    p_propertygrid->Append( new wxBoolProperty( "enabletargetclear", wxPG_LABEL, props["enabletargetclear"].GetPropValue<bool>() ) );

    if( props["enabletargetclear"].GetPropValue<bool>() )
    {
        wxPGProperty* targetclearcolor_prop = p_propertygrid->Append( new wxStringProperty( "targetclearcolor", wxPG_LABEL, "<composed>" ) );
        p_propertygrid->AppendIn( targetclearcolor_prop, new wxIntProperty( "r", wxPG_LABEL, props["targetclearcolor"].GetPropValue<unsigned char>( "r" ) ) );
        p_propertygrid->AppendIn( targetclearcolor_prop, new wxIntProperty( "g", wxPG_LABEL, props["targetclearcolor"].GetPropValue<unsigned char>( "g" ) ) );
        p_propertygrid->AppendIn( targetclearcolor_prop, new wxIntProperty( "b", wxPG_LABEL, props["targetclearcolor"].GetPropValue<unsigned char>( "b" ) ) );
    }

    bool viewportquad = props["viewportquad"].GetPropValue<bool>();

    p_propertygrid->Append( new wxBoolProperty( "viewportquad", wxPG_LABEL, viewportquad ) );

    if( viewportquad )
    {
        p_propertygrid->Append( new wxStringProperty( "viewportquad_fx", wxPG_LABEL, props["viewportquad_fx"].GetPropValue<dsstring>() ) );
        
        std::vector<std::pair<long, Pass::TextureSourceName>> viewportquad_textures;
        viewportquad_textures = props["viewportquad_textures"].GetPropValue<std::vector<std::pair<long, Pass::TextureSourceName>>>();

        if( viewportquad_textures.size() > 0 )
        {
            wxPGProperty* vpqtextures_prop = p_propertygrid->Append( new wxStringProperty( "viewportquad_textures", wxPG_LABEL, "<composed>" ) );

            for( size_t i = 0; i < viewportquad_textures.size(); i++ )
            {
                /*
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
                */

                Pass::TextureSourceName texture_source_name = viewportquad_textures[i].second;
                long stage = viewportquad_textures[i].first;

                char stage_entry[32];
                sprintf( stage_entry, "stage %d", stage );
                wxPGProperty* vpqtexturestage_prop = p_propertygrid->AppendIn( vpqtextures_prop, new wxStringProperty( stage_entry, wxPG_LABEL, "<composed>" ) );

                if( Pass::PASS_NAME == texture_source_name.source )
                {
                    p_propertygrid->AppendIn( vpqtexturestage_prop, new wxStringProperty( "Source type", wxPG_LABEL, "Pass" ) );
                }
                else
                {
                    p_propertygrid->AppendIn( vpqtexturestage_prop, new wxStringProperty( "Source type", wxPG_LABEL, "Texture" ) );
                }

                p_propertygrid->AppendIn( vpqtexturestage_prop, new wxStringProperty( "Source name", wxPG_LABEL, texture_source_name.name.c_str() ) );
            }

        }

        std::map<dsstring, RenderingNode::ShadersParams> viewportquad_shaderparams = props["viewportquad_shaderparams"].GetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>();

        if( viewportquad_shaderparams.size() )
        {
            wxPGProperty* vpqshaderparams_prop = p_propertygrid->Append( new wxStringProperty( "viewportquad_shaderparams", wxPG_LABEL, "<composed>" ) );

            for( std::map<dsstring, RenderingNode::ShadersParams>::iterator it = viewportquad_shaderparams.begin(); it != viewportquad_shaderparams.end(); ++ it )
            {
                /*
                char param_entry[128];
                sprintf( param_entry, "shaderparams/%s, shader %d register %d", it->first.c_str(), it->second.shader_index, it->second.param_register );

                char param_value[64];

                sprintf( param_value, "%.2f %.2f %.2f %.2f", it->second.param_values[0], it->second.param_values[1], it->second.param_values[2], it->second.param_values[3] );

                p_propertygrid->Append( new wxStringProperty( param_entry, wxPG_LABEL, param_value ) );
                */

                wxPGProperty* vpqshaderparamname_prop = p_propertygrid->AppendIn( vpqshaderparams_prop, new wxStringProperty( it->first.c_str(), wxPG_LABEL, "<composed>" ) );

                p_propertygrid->AppendIn( vpqshaderparamname_prop, new wxIntProperty( "shader index", wxPG_LABEL, it->second.shader_index ) );
                p_propertygrid->AppendIn( vpqshaderparamname_prop, new wxIntProperty( "register", wxPG_LABEL, it->second.param_register ) );

                wxPGProperty* vpqshaderparamvalue_prop = p_propertygrid->AppendIn( vpqshaderparamname_prop, new wxStringProperty( "value", wxPG_LABEL, "<composed>" ) );

                p_propertygrid->AppendIn( vpqshaderparamvalue_prop, new wxFloatProperty( "x", wxPG_LABEL, it->second.param_values[0] ) );
                p_propertygrid->AppendIn( vpqshaderparamvalue_prop, new wxFloatProperty( "y", wxPG_LABEL, it->second.param_values[1] ) );
                p_propertygrid->AppendIn( vpqshaderparamvalue_prop, new wxFloatProperty( "z", wxPG_LABEL, it->second.param_values[2] ) );
                p_propertygrid->AppendIn( vpqshaderparamvalue_prop, new wxFloatProperty( "w", wxPG_LABEL, it->second.param_values[3] ) );

            }
        }
    }
}

void wxWidgetAdapter::AdaptPassShaderValuesProps( Pass* p_pass, char* p_param_id, BasicSceneObjectPropertiesDialog* p_dialog )
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


    // release string allocated in wxWidgetAdapter::AdaptPassShaderValuesProps()
    delete[] param_id;


    // update mainframe list ctrl
    wxListCtrl* ctrl = (wxListCtrl*)p_dialog->GetData( "ctrl" );
    AdaptPassesShaderParamsList( pass, ctrl );

    p_dialog->Close();
}

void wxWidgetAdapter::AdaptLinearMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    propertygrid->Append( new wxStringProperty( "Alias", wxPG_LABEL, "" ) );


    wxPGProperty* initpos_prop = propertygrid->Append( new wxStringProperty( "Initial position", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( initpos_prop, new wxFloatProperty( "x", wxPG_LABEL, 0.0 ) );
    propertygrid->AppendIn( initpos_prop, new wxFloatProperty( "y", wxPG_LABEL, 0.0 ) );
    propertygrid->AppendIn( initpos_prop, new wxFloatProperty( "z", wxPG_LABEL, 0.0 ) );

    wxPGProperty* direction_prop = propertygrid->Append( new wxStringProperty( "Direction", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( direction_prop, new wxFloatProperty( "x", wxPG_LABEL, 1.0 ) );
    propertygrid->AppendIn( direction_prop, new wxFloatProperty( "y", wxPG_LABEL, 0.0 ) );
    propertygrid->AppendIn( direction_prop, new wxFloatProperty( "z", wxPG_LABEL, 0.0 ) );


    propertygrid->Append( new wxFloatProperty( "Theta", wxPG_LABEL, 0.0 ) );
    propertygrid->Append( new wxFloatProperty( "Phi", wxPG_LABEL, 0.0 ) );

    p_dialog->RegisterApplyButtonHandler( m_applylinearmvtvalues_callback );

}

void wxWidgetAdapter::on_applylinearmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    wxFloatProperty* prop;
    wxStringProperty* prop2;
    wxAny value;

    dsreal rval;
    Vector init_pos;
    Vector direction;
    dsreal theta, phi;

    dsstring alias;
    wxString alias2;
    wxCharBuffer buffer;

    prop2 = static_cast<wxStringProperty*>( propertygrid->GetProperty( "Alias" ) );
    value = prop2->GetValue();
    value.GetAs<wxString>( &alias2 );
    buffer = alias2.ToAscii();
    alias = buffer.data();

    if( "" == alias )
    {
        wxMessageBox( "'Alias' attribute cannot be void", "DrawFront error", wxICON_ERROR );
        return;
    }

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Initial position.x" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    init_pos[0] = rval;

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Initial position.y" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    init_pos[1] = rval;

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Initial position.z" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    init_pos[2] = rval;

    init_pos[3] = 1.0;

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Direction.x" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    direction[0] = rval;

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Direction.y" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    direction[1] = rval;

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Direction.z" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    direction[2] = rval;

    direction[3] = 1.0;

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Theta" ) );
    value = prop->GetValue();
    value.GetAs<double>( &theta );

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Phi" ) );
    value = prop->GetValue();
    value.GetAs<double>( &phi );

    LinearMovement* linear_mvt = new LinearMovement();
    linear_mvt->Init( init_pos, direction, theta, phi );

    std::map<dsstring, DrawSpace::Core::Movement*>* mvts_map = (std::map<dsstring, DrawSpace::Core::Movement*>*)p_dialog->GetData( "mvts_map" );

    (*mvts_map)[alias] = linear_mvt;

    wxListCtrl* ctrl = (wxListCtrl*)p_dialog->GetData( "ctrl" );

    AdaptMvtsList( mvts_map, ctrl );

    p_dialog->Close();
}

void wxWidgetAdapter::AdaptCircularMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    propertygrid->Append( new wxStringProperty( "Alias", wxPG_LABEL, "" ) );

    wxPGProperty* centerpos_prop = propertygrid->Append( new wxStringProperty( "Center position", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( centerpos_prop, new wxFloatProperty( "x", wxPG_LABEL, 0.0 ) );
    propertygrid->AppendIn( centerpos_prop, new wxFloatProperty( "y", wxPG_LABEL, 0.0 ) );
    propertygrid->AppendIn( centerpos_prop, new wxFloatProperty( "z", wxPG_LABEL, 0.0 ) );

    wxPGProperty* deltacenter_prop = propertygrid->Append( new wxStringProperty( "Delta center", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( deltacenter_prop, new wxFloatProperty( "x", wxPG_LABEL, 10.0 ) );
    propertygrid->AppendIn( deltacenter_prop, new wxFloatProperty( "y", wxPG_LABEL, 0.0 ) );
    propertygrid->AppendIn( deltacenter_prop, new wxFloatProperty( "z", wxPG_LABEL, 0.0 ) );

    wxPGProperty* rotaxis_prop = propertygrid->Append( new wxStringProperty( "Rotation axis", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( rotaxis_prop, new wxFloatProperty( "x", wxPG_LABEL, 0.0 ) );
    propertygrid->AppendIn( rotaxis_prop, new wxFloatProperty( "y", wxPG_LABEL, 1.0 ) );
    propertygrid->AppendIn( rotaxis_prop, new wxFloatProperty( "z", wxPG_LABEL, 0.0 ) );


    propertygrid->Append( new wxFloatProperty( "Initial angle", wxPG_LABEL, 0.0 ) );
    propertygrid->Append( new wxFloatProperty( "Theta", wxPG_LABEL, 0.0 ) );
    propertygrid->Append( new wxFloatProperty( "Phi", wxPG_LABEL, 0.0 ) );

    p_dialog->RegisterApplyButtonHandler( m_applycircularmvtvalues_callback );

}

void wxWidgetAdapter::on_applycircularmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    wxFloatProperty* prop;
    wxStringProperty* prop2;
    wxAny value;

    dsreal rval;
    Vector center_pos;
    Vector delta_center;
    Vector rot_axis;
    dsreal init_angle, theta, phi;

    dsstring alias;
    wxString alias2;
    wxCharBuffer buffer;


    prop2 = static_cast<wxStringProperty*>( propertygrid->GetProperty( "Alias" ) );
    value = prop2->GetValue();
    value.GetAs<wxString>( &alias2 );
    buffer = alias2.ToAscii();
    alias = buffer.data();

    if( "" == alias )
    {
        wxMessageBox( "'Alias' attribute cannot be void", "DrawFront error", wxICON_ERROR );
        return;
    }

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Center position.x" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    center_pos[0] = rval;

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Center position.y" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    center_pos[1] = rval;

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Center position.z" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    center_pos[2] = rval;

    center_pos[3] = 1.0;


    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Delta center.x" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    delta_center[0] = rval;

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Delta center.y" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    delta_center[1] = rval;

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Delta center.z" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    delta_center[2] = rval;

    delta_center[3] = 1.0;


    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Rotation axis.x" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    rot_axis[0] = rval;

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Rotation axis.y" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    rot_axis[1] = rval;

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Rotation axis.z" ) );
    value = prop->GetValue();
    value.GetAs<double>( &rval );
    rot_axis[2] = rval;

    rot_axis[3] = 1.0;


    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Initial angle" ) );
    value = prop->GetValue();
    value.GetAs<double>( &init_angle );

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Theta" ) );
    value = prop->GetValue();
    value.GetAs<double>( &theta );

    prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( "Phi" ) );
    value = prop->GetValue();
    value.GetAs<double>( &phi );

    CircularMovement* circular_mvt = new CircularMovement();
    circular_mvt->Init( center_pos, delta_center, rot_axis, init_angle, theta, phi );

    std::map<dsstring, DrawSpace::Core::Movement*>* mvts_map = (std::map<dsstring, DrawSpace::Core::Movement*>*)p_dialog->GetData( "mvts_map" );

    (*mvts_map)[alias] = circular_mvt;

    wxListCtrl* ctrl = (wxListCtrl*)p_dialog->GetData( "ctrl" );

    AdaptMvtsList( mvts_map, ctrl );

    p_dialog->Close();

}

void wxWidgetAdapter::AdaptFPSMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    propertygrid->Append( new wxStringProperty( "Alias", wxPG_LABEL, "" ) );

    wxPGProperty* initpos_prop = propertygrid->Append( new wxStringProperty( "Initial position", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( initpos_prop, new wxFloatProperty( "x", wxPG_LABEL, 0.0 ) );
    propertygrid->AppendIn( initpos_prop, new wxFloatProperty( "y", wxPG_LABEL, 0.0 ) );
    propertygrid->AppendIn( initpos_prop, new wxFloatProperty( "z", wxPG_LABEL, 0.0 ) );


    propertygrid->Append( new wxFloatProperty( "Initial yaw", wxPG_LABEL, 0.0 ) );
    propertygrid->Append( new wxFloatProperty( "Initial pitch", wxPG_LABEL, 0.0 ) );

    p_dialog->RegisterApplyButtonHandler( m_applyfpsmvtvalues_callback );
}

void wxWidgetAdapter::on_applyfpsmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog )
{

}

void wxWidgetAdapter::AdaptFreeMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    propertygrid->Append( new wxStringProperty( "Alias", wxPG_LABEL, "" ) );

    wxPGProperty* initpos_prop = propertygrid->Append( new wxStringProperty( "Initial position", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( initpos_prop, new wxFloatProperty( "x", wxPG_LABEL, 0.0 ) );
    propertygrid->AppendIn( initpos_prop, new wxFloatProperty( "y", wxPG_LABEL, 0.0 ) );
    propertygrid->AppendIn( initpos_prop, new wxFloatProperty( "z", wxPG_LABEL, 0.0 ) );


    p_dialog->RegisterApplyButtonHandler( m_applyfreemvtvalues_callback );
}

void wxWidgetAdapter::on_applyfreemvtvalues( BasicSceneObjectPropertiesDialog* p_dialog )
{

}

void wxWidgetAdapter::AdaptHeadMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    propertygrid->Append( new wxStringProperty( "Alias", wxPG_LABEL, "" ) );

    propertygrid->Append( new wxFloatProperty( "Scale factor", wxPG_LABEL, 0.0 ) );
    propertygrid->Append( new wxFloatProperty( "Ref force", wxPG_LABEL, 0.0 ) );

    wxPGProperty* headpos_prop = propertygrid->Append( new wxStringProperty( "Head position", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( headpos_prop, new wxFloatProperty( "x", wxPG_LABEL, 0.0 ) );
    propertygrid->AppendIn( headpos_prop, new wxFloatProperty( "y", wxPG_LABEL, 0.0 ) );
    propertygrid->AppendIn( headpos_prop, new wxFloatProperty( "z", wxPG_LABEL, 0.0 ) );


    p_dialog->RegisterApplyButtonHandler( m_applyheadmvtvalues_callback );
}

void wxWidgetAdapter::on_applyheadmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog )
{

}

void wxWidgetAdapter::AdaptSpectatorMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    propertygrid->Append( new wxStringProperty( "Alias", wxPG_LABEL, "" ) );

    propertygrid->Append( new wxFloatProperty( "Scale pos", wxPG_LABEL, 20.0 ) );
    propertygrid->Append( new wxIntProperty( "Period", wxPG_LABEL, 10 ) );
    propertygrid->Append( new wxBoolProperty( "Attached to Orbiter", wxPG_LABEL, false ) );

    p_dialog->RegisterApplyButtonHandler( m_applyspectatormvtvalues_callback );
}

void wxWidgetAdapter::on_applyspectatormvtvalues( BasicSceneObjectPropertiesDialog* p_dialog )
{

}

void wxWidgetAdapter::AdaptLongLatMvtCreationProps( BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    propertygrid->Append( new wxStringProperty( "Alias", wxPG_LABEL, "" ) );

    propertygrid->Append( new wxFloatProperty( "Initial longitud", wxPG_LABEL, 0.0 ) );
    propertygrid->Append( new wxFloatProperty( "Initial latitud", wxPG_LABEL, 0.0 ) );
    propertygrid->Append( new wxFloatProperty( "Initial altitud", wxPG_LABEL, 0.0 ) );
    propertygrid->Append( new wxFloatProperty( "Initial theta", wxPG_LABEL, 0.0 ) );
    propertygrid->Append( new wxFloatProperty( "Initial phi", wxPG_LABEL, 0.0 ) );

    p_dialog->RegisterApplyButtonHandler( m_applylonglatmvtvalues_callback );
}

void wxWidgetAdapter::on_applylonglatmvtvalues( BasicSceneObjectPropertiesDialog* p_dialog )
{

}


void wxWidgetAdapter::AdaptLinearMvtProps( const dsstring& p_mvtname, DrawSpace::Core::LinearMovement* p_movement, BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    Vector init_pos;
    Vector current_pos;
    Vector direction;
    dsreal current_theta;
    dsreal current_phi;

    p_movement->GetInitPos( init_pos );
    p_movement->GetDirection( direction );
    p_movement->GetCurrentPos( current_pos );
    current_theta = p_movement->GetCurrentTheta();
    current_phi = p_movement->GetCurrentPhi();

    propertygrid->Append( new wxStringProperty( "Alias", wxPG_LABEL, p_mvtname.c_str() ) );

    wxPGProperty* initpos_prop = propertygrid->Append( new wxStringProperty( "Initial position", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( initpos_prop, new wxFloatProperty( "x", wxPG_LABEL, init_pos[0] ) );
    propertygrid->AppendIn( initpos_prop, new wxFloatProperty( "y", wxPG_LABEL, init_pos[1] ) );
    propertygrid->AppendIn( initpos_prop, new wxFloatProperty( "z", wxPG_LABEL, init_pos[2] ) );


    wxPGProperty* direction_prop = propertygrid->Append( new wxStringProperty( "Direction", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( direction_prop, new wxFloatProperty( "x", wxPG_LABEL, direction[0] ) );
    propertygrid->AppendIn( direction_prop, new wxFloatProperty( "y", wxPG_LABEL, direction[1] ) );
    propertygrid->AppendIn( direction_prop, new wxFloatProperty( "z", wxPG_LABEL, direction[2] ) );

    wxPGProperty* currentpos_prop = propertygrid->Append( new wxStringProperty( "Current position", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( currentpos_prop, new wxFloatProperty( "x", wxPG_LABEL, current_pos[0] ) );
    propertygrid->AppendIn( currentpos_prop, new wxFloatProperty( "y", wxPG_LABEL, current_pos[1] ) );
    propertygrid->AppendIn( currentpos_prop, new wxFloatProperty( "z", wxPG_LABEL, current_pos[2] ) );

    propertygrid->Append( new wxFloatProperty( "Current theta", wxPG_LABEL, current_theta ) );
    propertygrid->Append( new wxFloatProperty( "Current phi", wxPG_LABEL, current_phi ) );

}

void wxWidgetAdapter::AdaptCircularMvtProps( const dsstring& p_mvtname, DrawSpace::Core::CircularMovement* p_movement, BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

    Vector center_pos;
    Vector delta_center;
    Vector rot_axis;
    dsreal init_angle;
    dsreal current_angle;
    dsreal current_theta;
    dsreal current_phi;

    p_movement->GetCenterPos( center_pos );
    p_movement->GetDeltaCenter( delta_center );
    p_movement->GetRotAxis( rot_axis );
    init_angle = p_movement->GetInitAngle();
    current_angle = p_movement->GetCurrentAngle();
    current_theta = p_movement->GetCurrentTheta();
    current_phi = p_movement->GetCurrentPhi();

    propertygrid->Append( new wxStringProperty( "Alias", wxPG_LABEL, p_mvtname.c_str() ) );

    wxPGProperty* centerpos_prop = propertygrid->Append( new wxStringProperty( "Center position", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( centerpos_prop, new wxFloatProperty( "x", wxPG_LABEL, center_pos[0] ) );
    propertygrid->AppendIn( centerpos_prop, new wxFloatProperty( "y", wxPG_LABEL, center_pos[1] ) );
    propertygrid->AppendIn( centerpos_prop, new wxFloatProperty( "z", wxPG_LABEL, center_pos[2] ) );

    wxPGProperty* deltacenter_prop = propertygrid->Append( new wxStringProperty( "Delta center", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( deltacenter_prop, new wxFloatProperty( "x", wxPG_LABEL, delta_center[0] ) );
    propertygrid->AppendIn( deltacenter_prop, new wxFloatProperty( "y", wxPG_LABEL, delta_center[1] ) );
    propertygrid->AppendIn( deltacenter_prop, new wxFloatProperty( "z", wxPG_LABEL, delta_center[2] ) );

    wxPGProperty* rotaxis_prop = propertygrid->Append( new wxStringProperty( "Rotation axis", wxPG_LABEL, "<composed>" ) );
    propertygrid->AppendIn( rotaxis_prop, new wxFloatProperty( "x", wxPG_LABEL, rot_axis[0] ) );
    propertygrid->AppendIn( rotaxis_prop, new wxFloatProperty( "y", wxPG_LABEL, rot_axis[1] ) );
    propertygrid->AppendIn( rotaxis_prop, new wxFloatProperty( "z", wxPG_LABEL, rot_axis[2] ) );


    propertygrid->Append( new wxFloatProperty( "Initial angle", wxPG_LABEL, init_angle ) );
    propertygrid->Append( new wxFloatProperty( "Current angle", wxPG_LABEL, current_angle ) );
    propertygrid->Append( new wxFloatProperty( "Current theta", wxPG_LABEL, current_theta ) );
    propertygrid->Append( new wxFloatProperty( "Current phi", wxPG_LABEL, current_phi ) );
}