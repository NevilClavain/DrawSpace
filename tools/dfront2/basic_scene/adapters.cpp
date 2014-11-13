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

void AdaptAssetsList( wxListCtrl* p_listctrl )
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

void AdaptConfigsList( wxListCtrl* p_listctrl )
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
    std::map<dsstring, std::pair<dsstring, dsstring>> texts_list;

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

void AdaptTextureProps( DrawSpace::Core::Texture* p_texture, wxPropertyGrid* p_propertygrid )
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

void AdaptShaderProps( DrawSpace::Core::Shader* p_shader, wxPropertyGrid* p_propertygrid )
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

void AdaptFontProps( DrawSpace::Core::Font* p_font, wxPropertyGrid* p_propertygrid )
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

void AdaptMesheProps( DrawSpace::Core::Meshe* p_meshe, wxPropertyGrid* p_propertygrid )
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

void AdaptFxProps( DrawSpace::Core::Fx* p_fx, wxPropertyGrid* p_propertygrid )
{
    Configurable::PropertiesMap props;
    p_fx->GetPropertiesMap( props );

    std::vector<dsstring> shaders_list = props["shaders"].GetPropValue<std::vector<dsstring>>();
    for( size_t i = 0; i < shaders_list.size(); i++ )
    {
        char shader_index[32];
        sprintf( shader_index, "shader:%d", i );
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
        sprintf( rsin_index, "renderstate_in:%d", i );
        
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

void AdaptPassProps( bool p_intermediate_pass, DrawSpace::Pass* p_pass, wxPropertyGrid* p_propertygrid )
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

}