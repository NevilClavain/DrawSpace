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

Spacebox::Spacebox( void ) :
m_scenegraph( NULL ),
m_scenenodegraph( NULL ),
m_enable_translations( false ),
m_ignore_camera( false )
{        
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
void Spacebox::SetDrawingState( const dsstring& p_passname, bool p_drawing )
{
    if( m_passesnodes.count( p_passname ) > 0 )
    {
        for( size_t i = 0; i < 6; i++ )
        {
            m_passesnodes[p_passname].nodes[i]->SetDrawingState( p_drawing );
        }
        return;
    }

    dsstring msg = "Spacebox : pass '";
    msg += p_passname;
    msg += "' unknown";

    _DSEXCEPTION( msg )   

}
*/

void Spacebox::SetDrawingState( Pass* p_pass, bool p_drawing )
{
    if( m_passesnodes.count( p_pass ) > 0 )
    {
        for( size_t i = 0; i < 6; i++ )
        {
            m_passesnodes[p_pass].nodes[i]->SetDrawingState( p_drawing );
        }
        return;
    }

    dsstring msg = "Spacebox : unknown pass '";
    _DSEXCEPTION( msg )   

}

void Spacebox::SetQuadDrawingState( Pass* p_pass, int p_quad, bool p_drawing )
{
    if( m_passesnodes.count( p_pass ) > 0 )
    {
        m_passesnodes[p_pass].nodes[p_quad]->SetDrawingState( p_drawing );
        return;
    }

    dsstring msg = "Spacebox : unknown pass '";
    _DSEXCEPTION( msg )
}

void Spacebox::OnRegister( Scenegraph* p_scenegraph )
{
    /*
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
    */
}

void Spacebox::OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
    //for( std::map<dsstring, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    for( std::map<Pass*, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        /*
        Pass* current_pass = p_scenegraph->GetPass( (*it).first );

        if( NULL == current_pass )
        {
            dsstring msg = "Spacebox : pass '";
            msg += (*it).first;
            msg += "' does not exists in scenegraph";

            _DSEXCEPTION( msg )
        }
        */

        Pass* current_pass = it->first;

        for( long i = 0; i < 6; i++ )
        {
            current_pass->GetRenderingQueue()->Add( (*it).second.nodes[i] );
        }
    }
    m_scenenodegraph = p_scenegraph;
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

    if( !m_enable_translations )
    {
        world.ClearTranslation();
    }

    view.Identity();

    if( !m_ignore_camera )
    {
        if( m_scenenodegraph )
        {
            m_scenenodegraph->GetCurrentCameraView( view );
        }
        else
        {
            m_scenegraph->GetCurrentCameraView( view );
        }
        view.ClearTranslation();
    }

    DrawSpace::Utils::Matrix proj;

    if( m_scenenodegraph )
    {
        m_scenenodegraph->GetCurrentCameraProj( proj );
    }
    else
    {
        m_scenegraph->GetCurrentCameraProj( proj );
    }

    m_renderer->DrawMeshe( p_rendering_node->GetMeshe()->GetVertexListSize(), p_rendering_node->GetMeshe()->GetTrianglesListSize(), world, view, proj );
}

//void Spacebox::RegisterPassSlot( const dsstring p_passname )
void Spacebox::RegisterPassSlot( Pass* p_pass )
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

    //m_passesnodes[p_passname] = nodeset;

    m_passesnodes[p_pass] = nodeset;
}

//DrawSpace::Core::RenderingNode* Spacebox::GetNodeFromPass( const dsstring& p_passname, int p_quadid )
DrawSpace::Core::RenderingNode* Spacebox::GetNodeFromPass( Pass* p_pass, int p_quadid )
{
    //if( 0 == m_passesnodes.count( p_passname ) )
    if( 0 == m_passesnodes.count( p_pass ) )
    {
        return NULL;
    }

    //return m_passesnodes[p_passname].nodes[p_quadid];

    return m_passesnodes[p_pass].nodes[p_quadid];
}

void Spacebox::GetBaseTransform( DrawSpace::Utils::Matrix& p_mat )
{
    p_mat = m_localtransformation;
}

void Spacebox::SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat )
{
    m_globaltransformation = p_mat;
}

void Spacebox::EnableTranslations( bool p_enable )
{
    m_enable_translations = p_enable;
}

void Spacebox::IgnoreCamera( bool p_ignore )
{
    m_ignore_camera = p_ignore;
}