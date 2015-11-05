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
m_diameter( p_diameter ),
m_current_face( -1 ),
m_current_patch( NULL )
{
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i] = _DRAWSPACE_NEW_( Face, Face( m_diameter ) );        
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
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->Init( i );
    }
}

void Body::Compute( void )
{
    // determiner la "face courante";
    
    //////////////////////////////////////
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->ResetDisplayList();
        bool status = m_faces[i]->ComputeAlignmentFactor();
    }

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
    m_faces[m_current_face]->Compute();

    check_currentpatch_event( m_faces[m_current_face]->GetCurrentPatch(), m_faces[m_current_face]->GetCurrentPatchLOD() );
}


void Body::BuildMeshe( void )
{
    m_planetpatch_meshe = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );

    dsreal xcurr, ycurr;
    long patch_resolution = PATCH_RESOLUTION;

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
            vertex.tv[0] = 1.0 - current_v0; // coin inferieur gauche de la grille correspond a la coord texture u = 0.0, v = 1.0 !!!!
                                            // le v des coords textures et le y du repere patch sont en sens opposés
            // inutile
            /*
            vertex.tu[1] = current_u0;
            vertex.tv[1] = current_v0;
            */

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

void Body::RegisterPatchUpdateHandler( PatchUpdateHandler* p_handler )
{
    m_patchupdate_handlers.push_back( p_handler );
}

void Body::UpdateHotPoint( const DrawSpace::Utils::Vector& p_hotpoint )
{    
    DrawSpace::Utils::Vector hotpoint = p_hotpoint;
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->UpdateRelativeHotpoint( hotpoint );
    }

    // compute altitud
    m_hotpoint_altitud = hotpoint.Length() - ( m_diameter / 2.0 );
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

dsreal Body::GetHotPointAltitud( void )
{
    return m_hotpoint_altitud;
}

int Body::GetCurrentFace (void )
{
    return m_current_face;
}
Face* Body::GetFace( int p_faceid )
{
    return m_faces[p_faceid];
}

void Body::SetHotState( bool p_hotstate )
{
    check_currentpatch_event( NULL, -1 );
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->SetHotState( p_hotstate );
    }
}

void Body::UpdateRelativeAlt( dsreal p_alt )
{
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->UpdateRelativeAlt( p_alt );
    }
}

void Body::check_currentpatch_event( Patch* p_newvalue, int p_currentpatch_lod )
{
    if( m_current_patch != p_newvalue )
    {
        m_current_patch = p_newvalue;
        for( size_t i = 0; i < m_patchupdate_handlers.size(); i++ )
        {
            (*m_patchupdate_handlers[i])( m_current_patch, p_currentpatch_lod ); 
        }
    }
}
