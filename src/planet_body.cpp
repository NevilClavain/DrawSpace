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

#include "planet_body.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Planet;


Body::Body( const dsstring& p_name ) : TransformNode( p_name ), m_scenegraph( NULL )
{
}

Body::~Body( void )
{
}

void Body::OnRegister( Scenegraph* p_scenegraph )
{
    for( std::map<dsstring, FacesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        Pass* current_pass = p_scenegraph->GetPass( (*it).first );
        if( current_pass != NULL )
        {         
            for( long i = 0; i < 6; i++ )
            {
                current_pass->GetRenderingQueue()->Add( (*it).second.faces[i] );
            }
        }
    }
    m_scenegraph = p_scenegraph;
}

void Body::on_renderingnode_draw( Core::RenderingNode* p_rendering_node )
{
    DrawSpace::Utils::Matrix view;
    m_scenegraph->GetCurrentCameraView( view );

    Face* face = static_cast<Face*>( p_rendering_node );

    face->Draw( m_globaltransformation, view );
}

void Body::RegisterPassFaceSet( const dsstring p_passname )
{
    FacesSet faceset;
    for( long i = 0; i < 6; i++ )
    {
        faceset.faces[i] = _DRAWSPACE_NEW_( Face, Face );
        RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Body::on_renderingnode_draw ) );

        faceset.faces[i]->RegisterHandler( cb );
        m_callbacks.push_back( cb );
    }

    m_passesnodes[p_passname] = faceset;
}

Fx* Body::GetPassFaceFx( const dsstring& p_passname, int p_faceid )
{
    if( 0 == m_passesnodes.count( p_passname ) )
    {
        return NULL;
    }
    FacesSet faceset = m_passesnodes[p_passname];

    return faceset.faces[p_faceid]->GetFx();
}