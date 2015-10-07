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

#include "renderer.h"
#include "plugin.h"
#include "memalloc.h"
#include "quadtree.h"
#include "exceptions.h"
#include "spherelod_body.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::SphericalLOD;

Meshe* Body::m_planetpatch_meshe = NULL;


Body::Body( dsreal p_diameter ) : 
m_diameter( p_diameter )
{
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i] = _DRAWSPACE_NEW_( Face, Face );
        m_faces[i]->SetPlanetDiameter( m_diameter );
    }
}

Body::~Body( void )
{
    for( long i = 0; i < 6; i++ )
    {
        _DRAWSPACE_DELETE_( m_faces[i] );
    }
}

void Body::Initialize( void )
{
    m_fractal = _DRAWSPACE_NEW_( CFractal, CFractal( 3, 7789, 0.5, 2.0 ) );

    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->Init( i, m_fractal );
    }
}


void Body::Compute( void )
{
    // determiner la "face courante";

    int curr_face = 0;
    dsreal af = m_faces[0]->GetAlignmentFactor();

    for( long i = 1; i < 6; i++ )
    {
        if( m_faces[i]->GetAlignmentFactor() > af )
        {
            curr_face = i;
            af = m_faces[i]->GetAlignmentFactor();
        }
    }

    m_current_face = curr_face;

    //////////////////////////////////////

    /*
    bool status = 0;

    status = m_faces[Patch::FrontPlanetFace]->Compute() | status;
    status = m_faces[Patch::RearPlanetFace]->Compute() | status;
    status = m_faces[Patch::TopPlanetFace]->Compute() | status; 
    status = m_faces[Patch::BottomPlanetFace]->Compute() | status;
    status = m_faces[Patch::RightPlanetFace]->Compute() | status;
    status = m_faces[Patch::LeftPlanetFace]->Compute() | status;

    if( status )
    {
        for( std::vector<EventHandler*>::iterator it = m_evt_handlers.begin(); it != m_evt_handlers.end(); ++it )
        {
            ( **it )( this, m_current_face );
        }
    }
    */

    for( long i = 0; i < 6; i++ )
    {
        bool status = m_faces[i]->Compute();

        if( status && i == m_current_face )
        {
            for( std::vector<EventHandler*>::iterator it = m_evt_handlers.begin(); it != m_evt_handlers.end(); ++it )
            {
                ( **it )( this, m_current_face );
            }            
        }
    }
}


void Body::BuildMeshe( void )
{
    m_planetpatch_meshe = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );

    dsreal xcurr, ycurr;
    long patch_resolution = SphericalLOD::Patch::Resolution;

    // on travaille sur une sphere de rayon = 1.0, donc diametre = 2.0
    dsreal interval = 2.0 / ( patch_resolution - 1 );

    float delta_uv0 = 1.0f / ( patch_resolution - 1 );
    float current_u0 = 0.0f;
    float current_v0 = 0.0f;


    for( long i = 0; i < patch_resolution; i++ )
    {
        for( long j = 0; j < patch_resolution; j++ )
        {
            xcurr = j * interval - 1.0;
            ycurr = i * interval - 1.0;
                        
            Vertex vertex;
            vertex.x = xcurr;
            vertex.y = ycurr;
            vertex.z = 0.0;

            vertex.tu[0] = current_u0;
            vertex.tv[0] = current_v0;

            vertex.tu[1] = current_u0;
            vertex.tv[1] = current_v0;

            m_planetpatch_meshe->AddVertex( vertex );

            current_u0 += delta_uv0;
        }

        current_v0 += delta_uv0;
        current_u0 = 0.0;
    }

    long current_index = 0;

    for( long i = 0; i < patch_resolution - 1; i++  )
    {
        current_index = i * patch_resolution;

        for( long j = 0; j < patch_resolution - 1; j++ )
        {
            Triangle triangle;

            triangle.vertex1 = current_index;
            triangle.vertex2 = current_index + 1;
            triangle.vertex3 = current_index + patch_resolution;
            m_planetpatch_meshe->AddTriangle( triangle );
            
            triangle.vertex1 = current_index + 1;
            triangle.vertex2 = current_index + 1 + patch_resolution;
            triangle.vertex3 = current_index + patch_resolution;
            m_planetpatch_meshe->AddTriangle( triangle );
            
            current_index++;
        }        
    }
}

void Body::RegisterEventHandler( EventHandler* p_handler )
{
    m_evt_handlers.push_back( p_handler );
}

void Body::UpdateHotPoint( const DrawSpace::Utils::Vector& p_hotpoint )
{
    m_hotpoint = p_hotpoint;

    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->UpdateRelativeHotpoint( m_hotpoint );
    }

    // compute altitud
    m_altitud = m_hotpoint.Length() - ( m_diameter / 2.0 );
}


DrawSpace::Core::Meshe* Body::GetPatcheMeshe( void )
{
    return m_planetpatch_meshe;
}


Patch* Body::GetFaceCurrentLeaf( int p_faceid )
{
    QuadtreeNode<Patch>* current_leaf = m_faces[p_faceid]->GetCurrentLeaf();
    return current_leaf->GetContent();
}

dsreal Body::GetAltitud( void )
{
    return m_altitud;
}

void Body::ResetMeshes( void )
{
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->ResetMeshe();
    }
}
/*
Maps* Body::GetFaceMapsFactory( int p_faceid )
{
    return m_faces[p_faceid]->GetMapsFactory();
}
*/
