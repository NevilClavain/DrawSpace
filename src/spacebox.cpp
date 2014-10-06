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

#include "spacebox.h"
#include "memalloc.h"
#include "exceptions.h"
#include "configsbase.h"
#include "assetsbase.h"
#include "misc_utils.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Spacebox::Spacebox( void ) : /*m_renderer( NULL ),*/ m_scenegraph( NULL )
{
    // properties array creation
//    m_properties["configname"].AddPropValue<dsstring>( m_configname );

    m_properties["passes_fx"].AddProp<std::map<dsstring, dsstring>>();
    m_properties["passes_textures"].AddProp<std::map<dsstring, std::vector<std::pair<long, TexturesNameSet>>>>();
    m_properties["passes_order"].AddProp<std::map<dsstring, long>>();
        
    //////////////////////////////////

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    Vertex v1, v2, v3, v4;

    for( long i = 0; i < 6; i++ )
    {
        m_meshes[i] = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );
        switch( i )
        {
            case FrontQuad:

                v1.x = -0.5;
                v1.y = 0.5;
                v1.z = -0.5;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = 0.5;
                v2.y = 0.5;
                v2.z = -0.5;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = 0.5;
                v3.y = -0.5;
                v3.z = -0.5;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = -0.5;
                v4.y = -0.5;
                v4.z = -0.5;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;
                break;

            case RearQuad:

                v1.x = 0.5;
                v1.y = 0.5;
                v1.z = 0.5;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = -0.5;
                v2.y = 0.5;
                v2.z = 0.5;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = -0.5;
                v3.y = -0.5;
                v3.z = 0.5;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = 0.5;
                v4.y = -0.5;
                v4.z = 0.5;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;
                break;

            case LeftQuad:

                v1.x = -0.5;
                v1.y = 0.5;
                v1.z = 0.5;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = -0.5;
                v2.y = 0.5;
                v2.z = -0.5;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = -0.5;
                v3.y = -0.5;
                v3.z = -0.5;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = -0.5;
                v4.y = -0.5;
                v4.z = 0.5;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                break;

            case RightQuad:

                v1.x = 0.5;
                v1.y = 0.5;
                v1.z = -0.5;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = 0.5;
                v2.y = 0.5;
                v2.z = 0.5;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = 0.5;
                v3.y = -0.5;
                v3.z = 0.5;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = 0.5;
                v4.y = -0.5;
                v4.z = -0.5;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                break;

            case TopQuad:

                v1.x = -0.5;
                v1.y = 0.5;
                v1.z = 0.5;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = 0.5;
                v2.y = 0.5;
                v2.z = 0.5;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = 0.5;
                v3.y = 0.5;
                v3.z = -0.5;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = -0.5;
                v4.y = 0.5;
                v4.z = -0.5;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                break;

            case BottomQuad:

                v1.x = -0.5;
                v1.y = -0.5;
                v1.z = -0.5;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = 0.5;
                v2.y = -0.5;
                v2.z = -0.5;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = 0.5;
                v3.y = -0.5;
                v3.z = 0.5;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = -0.5;
                v4.y = -0.5;
                v4.z = 0.5;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                break;
        }

        m_meshes[i]->AddVertex( v1 );
        m_meshes[i]->AddVertex( v2 );
        m_meshes[i]->AddVertex( v3 );
        m_meshes[i]->AddVertex( v4 );

        m_meshes[i]->AddTriangle( Triangle( 0, 3, 1 ) );
        m_meshes[i]->AddTriangle( Triangle( 1, 3, 2 ) );
    }
}

Spacebox::~Spacebox( void )
{
}

/*
void Spacebox::SetRenderer( Renderer * p_renderer )
{
    m_renderer = p_renderer;
}
*/

void Spacebox::OnRegister( Scenegraph* p_scenegraph )
{
    for( std::map<dsstring, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        Pass* current_pass = p_scenegraph->GetPass( (*it).first );

        if( NULL == current_pass )
        {
            dsstring msg = "Spacebox : pass '";
            msg += (*it).first;
            msg += "' does not exists in scenegraph";

            _DSEXCEPTION( msg )
        }

        for( long i = 0; i < 6; i++ )
        {
            current_pass->GetRenderingQueue()->Add( (*it).second.nodes[i] );
        }
    }
    m_scenegraph = p_scenegraph;
}

Core::Meshe* Spacebox::GetMeshe( int p_mesheid )
{
    return m_meshes[p_mesheid];
}

void Spacebox::on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node )
{
    DrawSpace::Utils::Matrix view;
    DrawSpace::Utils::Matrix world;

    world = m_globaltransformation;
    world.ClearTranslation();
    m_scenegraph->GetCurrentCameraView( view );
    view.ClearTranslation();

    DrawSpace::Utils::Matrix proj;
    m_scenegraph->GetCurrentCameraProj( proj );

    m_renderer->DrawMeshe( p_rendering_node->GetMeshe()->GetVertexListSize(), p_rendering_node->GetMeshe()->GetTrianglesListSize(), world, view, proj );
}

void Spacebox::RegisterPassSlot( const dsstring p_passname )
{
    NodesSet nodeset;
    for( long i = 0; i < 6; i++ )
    {
        nodeset.nodes[i] = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );
        RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Spacebox::on_renderingnode_draw ) );

        nodeset.nodes[i]->SetMeshe( m_meshes[i] );

        nodeset.nodes[i]->RegisterHandler( cb );
        m_callbacks.push_back( cb );

    }

    m_passesnodes[p_passname] = nodeset;
}

DrawSpace::Core::RenderingNode* Spacebox::GetNodeFromPass( const dsstring& p_passname, int p_quadid )
{
    if( 0 == m_passesnodes.count( p_passname ) )
    {
        return NULL;
    }

    return m_passesnodes[p_passname].nodes[p_quadid];
}

void Spacebox::Serialize( Utils::Archive& p_archive  )
{

}

bool Spacebox::Unserialize( Utils::Archive& p_archive )
{
    return true;
}


bool Spacebox::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    /*
    if( "configname" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["configname"].SetPropValue<dsstring>( p_words[1] );
    }
    else*/ if( "passes_fx" == p_words[0] )
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
        if( p_words.size() < 9 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        std::map<dsstring, std::vector<std::pair<long, TexturesNameSet>>> passes_textures = m_properties["passes_textures"].GetPropValue<std::map<dsstring, std::vector<std::pair<long, TexturesNameSet>>>>();

        long stage = StringToInt( p_words[2] );
        TexturesNameSet names;

        names.texturenames[0] = p_words[3];
        names.texturenames[1] = p_words[4];
        names.texturenames[2] = p_words[5];
        names.texturenames[3] = p_words[6];
        names.texturenames[4] = p_words[7];
        names.texturenames[5] = p_words[8];

        passes_textures[p_words[1]].push_back( std::pair<long, TexturesNameSet>( stage, names ) );

        m_properties["passes_textures"].SetPropValue<std::map<dsstring, std::vector<std::pair<long, TexturesNameSet>>>>( passes_textures );
    }
    else if( "passes_order" == p_words[0] )
    {
        if( p_words.size() < 3 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        std::map<dsstring, long> passes_order = m_properties["passes_order"].GetPropValue<std::map<dsstring, long>>();

        passes_order[p_words[1]] = StringToInt( p_words[2] );

        m_properties["passes_order"].SetPropValue<std::map<dsstring, long>>( passes_order );
    }
    else
    {
        _PARSER_UNEXPECTED_KEYWORD_
        return false;
    }

    return true;
}

void Spacebox::DumpProperties( dsstring& p_text )
{
    dsstring text_value;

    //p_text = "declare_config ";
    //p_text += dsstring( SPACEBOX_TEXT_KEYWORD );

    //p_text += "\n";
/*
    p_text += "configname ";
    p_text += m_properties["configname"].GetPropValue<dsstring>();
    p_text += "\n";
*/
    std::map<dsstring, dsstring> passes_fx = m_properties["passes_fx"].GetPropValue<std::map<dsstring, dsstring>>();
    for( std::map<dsstring, dsstring>::iterator it = passes_fx.begin(); it != passes_fx.end(); ++it )
    {
        p_text += "passes_fx ";

        p_text += it->first;
        p_text += " ";
        p_text += it->second;
        p_text += "\n";
    }

    std::map<dsstring, std::vector<std::pair<long, TexturesNameSet>>> passes_textures = m_properties["passes_textures"].GetPropValue<std::map<dsstring, std::vector<std::pair<long, TexturesNameSet>>>>();
    for( std::map<dsstring, std::vector<std::pair<long, TexturesNameSet>>>::iterator it = passes_textures.begin(); it != passes_textures.end(); ++it )
    {       
        for( size_t i = 0; i < it->second.size(); i++ )
        {
            p_text += "passes_textures ";
            p_text += it->first;
            p_text += " ";

            std::pair<long, TexturesNameSet> name_sets = (it->second)[i];

            IntToString( name_sets.first, text_value );

            p_text += text_value;
            p_text += " ";

            for( long j = 0; j < 6; j++ )
            {
                p_text += name_sets.second.texturenames[i];
                p_text += " ";
            }
        }

        p_text += "\n";
    }

    std::map<dsstring, long> passes_order = m_properties["passes_order"].GetPropValue<std::map<dsstring, long>>();
    for( std::map<dsstring, long>::iterator it = passes_order.begin(); it != passes_order.end(); ++it )
    {
        p_text += "passes_order ";
        p_text += it->first;
        p_text += " ";
        
        IntToString( it->second, text_value );
        p_text += text_value;

        p_text += "\n";
    }

    //p_text += "end_config\n";
}

bool Spacebox::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}

void Spacebox::ApplyProperties( void )
{
    //m_configname = m_properties["configname"].GetPropValue<dsstring>();

    // create passes slots and set fx to each corresponding rendering nodes
    std::map<dsstring, dsstring> passes_fx = m_properties["passes_fx"].GetPropValue<std::map<dsstring, dsstring>>();
    for( std::map<dsstring, dsstring>::iterator it = passes_fx.begin(); it != passes_fx.end(); ++it )
    {
        RegisterPassSlot( it->first );
        NodesSet nodeset = m_passesnodes[it->first];
        dsstring fxname = it->second;

        Configurable* config = ConfigsBase::GetInstance()->GetConfigurable( fxname );

        Fx* fx = dynamic_cast<Fx*>( config );
        if( !fx )
        {
            _DSEXCEPTION( "Specified asset is not an Fx" );
        }
        
        for( long i = 0; i < 6; i++ )
        {
            nodeset.nodes[i]->SetFx( fx );
        }
    }

    std::map<dsstring, std::vector<std::pair<long, TexturesNameSet>>> passes_textures = m_properties["passes_textures"].GetPropValue<std::map<dsstring, std::vector<std::pair<long, TexturesNameSet>>>>();
    for( std::map<dsstring, std::vector<std::pair<long, TexturesNameSet>>>::iterator it = passes_textures.begin(); it != passes_textures.end(); ++it )
    {
        if( 0 == m_passesnodes.count( it->first ) )
        {
            _DSEXCEPTION( "Specified pass is not registered/don't exists" );
        }

        NodesSet nodeset = m_passesnodes[it->first];

        std::vector<std::pair<long, TexturesNameSet>> textures_set = it->second;

        for( size_t i = 0; i < textures_set.size(); i++ )
        {
            long stage = textures_set[i].first;
            TexturesNameSet nameset = textures_set[i].second;

            for( long j = 0; j < 6; j++ )
            {
                Asset* asset = AssetsBase::GetInstance()->GetAsset( nameset.texturenames[i] );

                Texture* texture = dynamic_cast<Texture*>( asset);
                if( !texture )
                {
                    _DSEXCEPTION( "Specified asset is not a texture" );
                }
                
                nodeset.nodes[i]->SetTexture( texture, stage );
            }
        }                
    }

    std::map<dsstring, long> passes_order = m_properties["passes_order"].GetPropValue<std::map<dsstring, long>>();
    for( std::map<dsstring, long>::iterator it = passes_order.begin(); it != passes_order.end(); ++it )
    {
        if( 0 == m_passesnodes.count( it->first ) )
        {
            _DSEXCEPTION( "Specified pass is not registered/don't exists" );
        }

        NodesSet nodeset = m_passesnodes[it->first];

        for( long i = 0; i < 6; i++ )
        {
            nodeset.nodes[i]->SetOrderNumber( it->second );
        }      
    }
}

Configurable* Spacebox::Instanciate( void )
{
    return _DRAWSPACE_NEW_( Spacebox, Spacebox );
}


