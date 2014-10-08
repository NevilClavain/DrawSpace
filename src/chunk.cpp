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

#include "exceptions.h"
#include "memalloc.h"
#include "chunk.h"
#include "exceptions.h"
#include "configsbase.h"
#include "assetsbase.h"
#include "misc_utils.h"


using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Chunk::Chunk( void ) : 
m_renderer( NULL ), 
m_scenegraph( NULL ), 
m_lod_draw( true ),
m_meshe( NULL )
{
    // properties array creation
    m_properties["meshe"].AddProp<dsstring>();
    m_properties["passes_fx"].AddProp<std::map<dsstring, dsstring>>();
    m_properties["passes_textures"].AddProp<std::map<dsstring, std::vector<std::pair<long, dsstring>>>>();


    //m_meshe = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );
    m_lod_callback = _DRAWSPACE_NEW_( LodCallback, LodCallback( this, &Chunk::on_lod_event ) );
}

Chunk::~Chunk( void )
{
    //_DRAWSPACE_DELETE_( m_meshe );
    _DRAWSPACE_DELETE_( m_lod_callback );
}


void Chunk::SetRenderer( Renderer * p_renderer )
{
    m_renderer = p_renderer;
}

void Chunk::OnRegister( Scenegraph* p_scenegraph )
{
    if( NULL == m_meshe )
    {
        _DSEXCEPTION( "NULL meshe ; please allocate a meshe object for chunk prior to other operations" );
    }

    for( std::map<dsstring, RenderingNode*>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        Pass* current_pass = p_scenegraph->GetPass( (*it).first );

        if( NULL == current_pass )
        {
            dsstring msg = "Chunk : pass '";
            msg += (*it).first;
            msg += "' does not exists in scenegraph";

            _DSEXCEPTION( msg )
        }

        current_pass->GetRenderingQueue()->Add( (*it).second );
    }
    m_scenegraph = p_scenegraph;


    dsstring vsphere_name = m_scenename + dsstring( "/vsphere" );
    m_vsphere = _DRAWSPACE_NEW_( VSphere, VSphere( vsphere_name ) );

    // virtual sphere characteristics
    dsreal vsphere_ray;
    Utils::Vector vsphere_point;

    Utils::Vector vmin, vmax;
    m_meshe->GetAABB( vmin, vmax );

    Utils::Vector delta;
    delta[0] = vmax[0] - vmin[0];
    delta[1] = vmax[1] - vmin[1];
    delta[2] = vmax[2] - vmin[2];

    vsphere_point[0] = ( delta[0] / 2.0 ) + vmin[0];
    vsphere_point[1] = ( delta[1] / 2.0 ) + vmin[1];
    vsphere_point[2] = ( delta[2] / 2.0 ) + vmin[2];

    long index_max = 0;
    double max_delta = delta[0];

    if( max_delta < delta[1] )
    {
        max_delta = delta[1];
        index_max = 1;
    }
    if( max_delta < delta[2] )
    {
        max_delta = delta[2];
        index_max = 2;
    }
   
    switch( index_max )
    {
        case 0:

            vsphere_ray = delta[0] / 2.0;
            break;

        case 1:

            vsphere_ray = delta[1] / 2.0;
            break;

        case 2:

            vsphere_ray = delta[2] / 2.0;
            break;

    }

    m_vsphere->SetRay( vsphere_ray );
    m_vsphere->SetPoint( vsphere_point );


    // LOD default settings
    LodStep* lodstep = _DRAWSPACE_NEW_( LodStep, LodStep( -1000.0, 1000.0, m_vsphere ) );
    lodstep->RegisterHandler( m_lod_callback );
    m_lodsteps.push_back( lodstep );
}

Core::Meshe* Chunk::GetMeshe( void )
{
    return m_meshe;
}

void Chunk::SetMeshe( DrawSpace::Core::Meshe* p_meshe )
{
    m_meshe = p_meshe;
}


void Chunk::on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node )
{
    DrawSpace::Utils::Matrix view;
    m_scenegraph->GetCurrentCameraView( view );

    DrawSpace::Utils::Matrix proj;
    m_scenegraph->GetCurrentCameraProj( proj );

    DrawSpace::Utils::Matrix res;
    res = m_globaltransformation * view;
    m_vsphere->Transform( res );

    if( false == m_lod_draw )
    {
        return;
    }

    bool draw = false;
    Utils::Vector transformed_vsphere_point;
    m_vsphere->GetTransformedPoint( transformed_vsphere_point );
    
    if( m_vsphere->Collide( Utils::Vector( 0.0, 0.0, 0.0, 1.0 ) ) )
    {
        draw = true;
    }
    else
    {
        if( transformed_vsphere_point[2] <= 0.0 )
        {
            draw = true;
        }
    }

    if( draw )
    {
        m_renderer->DrawMeshe( p_rendering_node->GetMeshe()->GetVertexListSize(), p_rendering_node->GetMeshe()->GetTrianglesListSize(), m_globaltransformation, view, proj );
    }
}

void Chunk::on_lod_event( LodStep*, LodStep::Event p_event )
{
    switch( p_event )
    {
        case LodStep::IN_LODSTEP:

            m_lod_draw = true;
            break;

        case LodStep::OUT_LODSTEP:

            m_lod_draw = false;
            break;
    }
}

void Chunk::RegisterPassSlot( const dsstring p_passname )
{
    m_passesnodes[p_passname] = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );

    if( NULL == m_meshe )
    {
        _DSEXCEPTION( "NULL meshe ; please allocate a meshe object for chunk prior to other operations" );
    }

    m_passesnodes[p_passname]->SetMeshe( m_meshe );

    RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Chunk::on_renderingnode_draw ) );

    m_passesnodes[p_passname]->RegisterHandler( cb );
    m_callbacks.push_back( cb );

}

DrawSpace::Core::RenderingNode* Chunk::GetNodeFromPass( const dsstring& p_passname )
{
    if( 0 == m_passesnodes.count( p_passname ) )
    {
        return NULL;
    }

    return m_passesnodes[p_passname];
}

bool Chunk::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "passes_fx" == p_words[0] )
    {
        if( p_words.size() < 3 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        std::map<dsstring, dsstring> passes_fx = m_properties["passes_fx"].GetPropValue<std::map<dsstring, dsstring>>();
        passes_fx[p_words[1]] = p_words[2];

        m_properties["passes_fx"].SetPropValue<std::map<dsstring, dsstring>>( passes_fx );

    }
    else if( "passes_textures" == p_words[0] )
    {
        if( p_words.size() < 4 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        std::map<dsstring, std::vector<std::pair<long, dsstring>>> passes_textures = m_properties["passes_textures"].GetPropValue<std::map<dsstring, std::vector<std::pair<long, dsstring>>>>();

        long stage = StringToInt( p_words[2] );
        dsstring texture_name = p_words[3];

        passes_textures[p_words[1]].push_back( std::pair<long, dsstring>( stage, texture_name ) );

        m_properties["passes_textures"].SetPropValue<std::map<dsstring, std::vector<std::pair<long, dsstring>>>>( passes_textures );
    }
    else if( "meshe" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["meshe"].SetPropValue<dsstring>( p_words[1] );
    }

    else
    {
        _PARSER_UNEXPECTED_KEYWORD_
        return false;
    }

    return true;
}

void Chunk::Serialize( Utils::Archive& p_archive  )
{
}

bool Chunk::Unserialize( Utils::Archive& p_archive )
{
    return true;
}

void Chunk::DumpProperties( dsstring& p_text )
{
    dsstring text_value;

    p_text = "meshe ";
    p_text += m_properties["meshe"].GetPropValue<dsstring>();
    p_text += "\n";

    std::map<dsstring, dsstring> passes_fx = m_properties["passes_fx"].GetPropValue<std::map<dsstring, dsstring>>();
    for( std::map<dsstring, dsstring>::iterator it = passes_fx.begin(); it != passes_fx.end(); ++it )
    {
        p_text += "passes_fx ";

        p_text += it->first;
        p_text += " ";
        p_text += it->second;
        p_text += "\n";
    }

    std::map<dsstring, std::vector<std::pair<long, dsstring>>> passes_textures = m_properties["passes_textures"].GetPropValue<std::map<dsstring, std::vector<std::pair<long, dsstring>>>>();
    for( std::map<dsstring, std::vector<std::pair<long, dsstring>>>::iterator it = passes_textures.begin(); it != passes_textures.end(); ++it )
    {       
        for( size_t i = 0; i < it->second.size(); i++ )
        {
            p_text += "passes_textures ";
            p_text += it->first;
            p_text += " ";

            std::pair<long, dsstring> name_sets = (it->second)[i];

            IntToString( name_sets.first, text_value );

            p_text += text_value;
            p_text += " ";
            p_text += name_sets.second;                
        }

        p_text += "\n";
    }
}

bool Chunk::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}

void Chunk::ApplyProperties( void )
{
    dsstring meshe_name = m_properties["meshe"].GetPropValue<dsstring>();

    if( false == AssetsBase::GetInstance()->AssetIdExists( meshe_name ) )
    {
        _DSEXCEPTION( "Meshe Asset id unknown in AssetsBase" );
    }
    
    Asset* asset = AssetsBase::GetInstance()->GetAsset( meshe_name );

    Meshe* meshe = dynamic_cast<Meshe*>( asset );
    if( !meshe )
    {
        _DSEXCEPTION( "Specified asset is not a Meshe" );
    }

    SetMeshe( meshe );

    // create passes slots and set fx to each corresponding rendering nodes
    std::map<dsstring, dsstring> passes_fx = m_properties["passes_fx"].GetPropValue<std::map<dsstring, dsstring>>();
    for( std::map<dsstring, dsstring>::iterator it = passes_fx.begin(); it != passes_fx.end(); ++it )
    {
        RegisterPassSlot( it->first );
        RenderingNode* rendering_node = m_passesnodes[it->first];
        dsstring fxname = it->second;

        if( false == ConfigsBase::GetInstance()->ConfigurableInstanceExists( fxname ) )
        {
            _DSEXCEPTION( "Config id unknown in ConfigsBase" );
        }

        Configurable* config = ConfigsBase::GetInstance()->GetConfigurableInstance( fxname );

        Fx* fx = dynamic_cast<Fx*>( config );
        if( !fx )
        {
            _DSEXCEPTION( "Specified asset is not an Fx" );
        }
        rendering_node->SetFx( fx );        
    }

    // textures for each passe rendering node

    std::map<dsstring, std::vector<std::pair<long, dsstring>>> passes_textures = m_properties["passes_textures"].GetPropValue<std::map<dsstring, std::vector<std::pair<long, dsstring>>>>();
    for( std::map<dsstring, std::vector<std::pair<long, dsstring>>>::iterator it = passes_textures.begin(); it != passes_textures.end(); ++it )
    {
        if( 0 == m_passesnodes.count( it->first ) )
        {
            _DSEXCEPTION( "Specified pass is not registered/don't exists" );
        }

        RenderingNode* rendering_node = m_passesnodes[it->first];

        std::vector<std::pair<long, dsstring>> textures_set = it->second;

        for( size_t i = 0; i < textures_set.size(); i++ )
        {
            long stage = textures_set[i].first;
            dsstring texture_name = textures_set[i].second;

            Asset* asset = AssetsBase::GetInstance()->GetAsset( texture_name );

            Texture* texture = dynamic_cast<Texture*>( asset );
            if( !texture )
            {
                _DSEXCEPTION( "Specified asset is not a texture" );
            }

            rendering_node->SetTexture( texture, stage );
        }
    }
}

Configurable* Chunk::Instanciate( void )
{
    return _DRAWSPACE_NEW_( Chunk, Chunk );
}
