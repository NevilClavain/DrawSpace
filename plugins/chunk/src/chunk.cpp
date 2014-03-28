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

#include <exceptions.h>
#include "chunk.h"
#include <memalloc.h>

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;

Chunk::Chunk( void ) : m_renderer( NULL ), m_scenegraph( NULL ), m_lod_draw( true )
{
    m_meshe = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );
    m_lod_callback = _DRAWSPACE_NEW_( LodCallback, LodCallback( this, &Chunk::on_lod_event ) );
}

Chunk::~Chunk( void )
{
    _DRAWSPACE_DELETE_( m_meshe );
    _DRAWSPACE_DELETE_( m_lod_callback );
}

void Chunk::Initialize( void )
{
}

void Chunk::GetDescr( dsstring& p_descr )
{
    p_descr = "Chunk";
}

void Chunk::DumpMemoryAllocs( void )
{
    DrawSpace::Utils::MemAlloc::GetInstance()->DumpContent();
}

void Chunk::SetRenderer( Renderer * p_renderer )
{
    m_renderer = p_renderer;
}

void Chunk::OnRegister( Scenegraph* p_scenegraph )
{
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

        /*
        if( current_pass != NULL )
        {
            current_pass->GetRenderingQueue()->Add( (*it).second );
        }
        */
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

Core::Meshe* Chunk::GetMeshe( const dsstring& p_mesheid )
{
    return m_meshe;
}

void Chunk::on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node )
{
    DrawSpace::Utils::Matrix view;
    m_scenegraph->GetCurrentCameraView( view );

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
        m_renderer->DrawMeshe( p_rendering_node->GetMeshe()->GetVertexListSize(), p_rendering_node->GetMeshe()->GetTrianglesListSize(), m_globaltransformation, view );
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

    m_passesnodes[p_passname]->SetMeshe( m_meshe );

    RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Chunk::on_renderingnode_draw ) );

    m_passesnodes[p_passname]->RegisterHandler( cb );
    m_callbacks.push_back( cb );

}

DrawSpace::Core::RenderingNode* Chunk::GetNodeFromPass( const dsstring& p_passname, const dsstring& p_nodeid )
{
    if( 0 == m_passesnodes.count( p_passname ) )
    {
        return NULL;
    }

    return m_passesnodes[p_passname];
}

void Chunk::GetNodesIdsList( std::vector<dsstring>& p_ids )
{
    p_ids.push_back( "" );
}

void Chunk::ComputeSpecifics( void )
{
}

void Chunk::SetNodeFromPassSpecificFx( const dsstring& p_passname, const dsstring& p_nodeid, const dsstring& p_fxname )
{
}

void Chunk::GetPropertiesList( std::vector<dsstring>& p_props )
{
}

DrawSpace::Core::Property* Chunk::GetProperty( const dsstring& p_name )
{
    return NULL;
}

void Chunk::SetProperty( const dsstring& p_name, DrawSpace::Core::Property* p_prop )
{
}
