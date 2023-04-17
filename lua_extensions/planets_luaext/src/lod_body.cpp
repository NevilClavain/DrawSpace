/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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
/* -*-LIC_END-*- */

#include "lod_body.h"
#include "csts.h"
#include "lod_config.h"
#include "renderer.h"
#include "plugin.h"
#include "memalloc.h"
#include "quadtree.h"
#include "exceptions.h"
#include "maths.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace LOD;

Meshe* Body::m_patch_meshe = NULL;  //patch terrains
Meshe* Body::m_patch2_meshe = NULL;  // patch terrains haute resolution
Meshe* Body::m_patch3_meshe = NULL;  // patch terrains moyenne resolution
Meshe* Body::m_skirt_meshe = NULL;  //les jupes terrains


Body::Body( Config* p_config, int p_layer_index, SubPass::SubPassCreationHandler* p_handler, int p_nbLODRanges, const dsstring& p_description) :
m_current_face( -1 ),
m_relative_alt( 0.0 ),
m_config( p_config ),
m_description(p_description)
{
    m_enable_cdlod = p_config->m_layers_descr[p_layer_index].enable_lod;
    m_diameter = 1000.0 * m_config->m_layers_descr[p_layer_index].ray * 2.0;

    for( long i = 0; i < 6; i++ )
    {
        m_faces[i] = _DRAWSPACE_NEW_( Face, Face( m_config, p_layer_index, p_handler, p_nbLODRanges ) );
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
    for( long i = 0; i < 6; i++ )
    {        
        m_faces[i]->ComputeAlignmentFactor();
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

    if( m_current_face != curr_face )
    {
        m_current_face = curr_face;
    }

    /////////////////////////////////////////

    double alignment_factor_limit;

    // alignment_factor_limit augmente au fur et a mesure qu'on approche l'altitude zero
    alignment_factor_limit = 0.25 * DrawSpace::Utils::Maths::Clamp( 0.0, 1.0, ( 3.0 - m_relative_alt ) / 3.0 );

    if( m_enable_cdlod )
    {
        for( long i = 0; i < 6; i++ )
        {
            if( m_faces[i]->GetAlignmentFactor() > alignment_factor_limit )
            {
                m_faces[i]->UpdateLODComputationParams();
                m_faces[i]->Compute();
                m_faces[i]->UpdateLODComputationResults();    
            }
            else
            {
                m_faces[i]->ResetDisplayList();
            }
        }
    }
}


void Body::build_meshe( long p_patch_resol, DrawSpace::Core::Meshe* p_meshe_dest, bool p_fastmode, bool p_skirt )
{
    int main_patch_nbv = 0;
    int right_skirt_nbv = 0;
    int left_skirt_nbv = 0;
    int up_skirt_nbv = 0;
    int down_skirt_nbv = 0;

    dsreal xcurr, ycurr;
    long patch_resolution = p_patch_resol;

    // on travaille sur une sphere de rayon = 1.0, donc diametre = 2.0
    dsreal interval = 2.0 / ( patch_resolution - 1 );

    float delta_uv0 = 1.0f / ( patch_resolution - 1 );
    float current_u0 = 0.0f;
    float current_v0 = 0.0f;

    if( !p_skirt )
    {
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
                                                // le v des coords textures et le y du repere patch sont en sens oppos�s

                vertex.tw[0] = 0.0;

                p_meshe_dest->AddVertex( vertex );
                main_patch_nbv++;

                current_u0 += delta_uv0;
            }

            current_v0 += delta_uv0;
            current_u0 = 0.0;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////
    
    //if( p_skirt )
    else
    {
        

        // vertices jupe droite

        current_u0 = 0.0f;
        current_v0 = 0.0f;

        for( long i = 0; i < patch_resolution; i++ )
        {
            for( long j = 0; j < 2; j++ )
            {
                xcurr = 1.0;
                ycurr = i * interval - 1.0;
                        
                Vertex vertex;
                vertex.x = xcurr;
                vertex.y = ycurr;
                vertex.z = 0.0;

                vertex.tu[0] = 1.0;
                vertex.tv[0] = 1.0 - current_v0; // coin inferieur gauche de la grille correspond a la coord texture u = 0.0, v = 1.0 !!!!
                                                // le v des coords textures et le y du repere patch sont en sens oppos�s

                if( 1 == j )
                {
                    vertex.tw[0] = 1.0; // info pour shader : ce vertex est en bord de jupe
                    vertex.x += 0.01;
                }
                else
                {
                    vertex.tw[0] = 0.0;
                }

                p_meshe_dest->AddVertex( vertex );  
                right_skirt_nbv++;
            }
            current_v0 += delta_uv0;
        }
 
        // vertices jupe gauche
        
        current_u0 = 0.0f;
        current_v0 = 0.0f;

        for( long i = 0; i < patch_resolution; i++ )
        {
            for( long j = 0; j < 2; j++ )
            {
                xcurr = -1.0;
                ycurr = i * interval - 1.0;
                        
                Vertex vertex;
                vertex.x = xcurr;
                vertex.y = ycurr;
                vertex.z = 0.0;

                vertex.tu[0] = 0.0;
                vertex.tv[0] = 1.0 - current_v0; // coin inferieur gauche de la grille correspond a la coord texture u = 0.0, v = 1.0 !!!!
                                                // le v des coords textures et le y du repere patch sont en sens oppos�s
                if( 0 == j )
                {
                    vertex.tw[0] = 1.0; // info pour shader : ce vertex est en bord de jupe
                    vertex.x -= 0.01;
                }
                else
                {
                    vertex.tw[0] = 0.0;
                }

                p_meshe_dest->AddVertex( vertex );  
                left_skirt_nbv++;
            }
            current_v0 += delta_uv0;
        }

        // vertices jupe up

        current_u0 = 0.0f;
        current_v0 = 0.0f;

        for( long i = 0; i < 2; i++ )
        {
            for( long j = 0; j < patch_resolution; j++ )
            {
                xcurr = j * interval - 1.0;
                ycurr = 1.0;
                        
                Vertex vertex;
                vertex.x = xcurr;
                vertex.y = ycurr;
                vertex.z = 0.0;

                vertex.tu[0] = current_u0;
                vertex.tv[0] = 0.0;
                            
                if( 1 == i )
                {
                    vertex.tw[0] = 1.0; // info pour shader : ce vertex est en bord de jupe
                    vertex.y += 0.01;
                }
                else
                {
                    vertex.tw[0] = 0.0;
                }

                p_meshe_dest->AddVertex( vertex );  
                up_skirt_nbv++;

                current_u0 += delta_uv0;
            }

            current_u0 = 0.0;
        }


        // vertices jupe down

        current_u0 = 0.0f;
        current_v0 = 0.0f;

        for( long i = 0; i < 2; i++ )
        {
            for( long j = 0; j < patch_resolution; j++ )
            {
                xcurr = j * interval - 1.0;
                ycurr = -1.0;
                        
                Vertex vertex;
                vertex.x = xcurr;
                vertex.y = ycurr;
                vertex.z = 0.0;

                vertex.tu[0] = current_u0;
                vertex.tv[0] = 1.0;
                            
                if( 0 == i )
                {
                    vertex.tw[0] = 1.0; // info pour shader : ce vertex est en bord de jupe
                    vertex.y -= 0.01;
                }
                else
                {
                    vertex.tw[0] = 0.0;
                }

                p_meshe_dest->AddVertex( vertex );  
                down_skirt_nbv++;

                current_u0 += delta_uv0;
            }

            current_u0 = 0.0;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////

    long current_index;

    if( !p_skirt )
    {
        for( long i = 0; i < patch_resolution - 1; i++  )
        {
            current_index = i * patch_resolution;

            for( long j = 0; j < patch_resolution - 1; j++ )
            {
                TrianglePrimitive<unsigned int> triangle;

                triangle.at(0) = current_index;
                triangle.at(1) = current_index + 1;
                triangle.at(2) = current_index + patch_resolution;
                p_meshe_dest->AddTriangle( triangle, p_fastmode );
            
                triangle.at(0) = current_index + 1;
                triangle.at(1) = current_index + 1 + patch_resolution;
                triangle.at(2) = current_index + patch_resolution;
                p_meshe_dest->AddTriangle( triangle, p_fastmode );
            
                current_index++;
            }        
        }
    }

    ///////////////////////////////////////////////////////////
    
    //if( p_skirt )
    else
    {

        // triangles jupe droite

        int right_skirt_base_index = main_patch_nbv;

        current_index = 0;

        for( long i = 0; i < patch_resolution - 1; i++ )
        {
            current_index = ( i * 2 ) + right_skirt_base_index;

            TrianglePrimitive<unsigned int> triangle;

            triangle.at(0) = current_index;
            triangle.at(1) = current_index + 1;
            triangle.at(2) = current_index + 2;
            p_meshe_dest->AddTriangle( triangle, p_fastmode );
            
            triangle.at(0) = current_index + 1;
            triangle.at(1) = current_index + 3;
            triangle.at(2) = current_index + 2;
            p_meshe_dest->AddTriangle( triangle, p_fastmode );            
        }

        // triangles jupe gauche

        int left_skirt_base_index = right_skirt_base_index + right_skirt_nbv;

        for( long i = 0; i < patch_resolution - 1; i++ )
        {
            current_index = ( i * 2 ) + left_skirt_base_index;

            TrianglePrimitive<unsigned int> triangle;

            triangle.at(0) = current_index;
            triangle.at(1) = current_index + 1;
            triangle.at(2) = current_index + 2;
            p_meshe_dest->AddTriangle( triangle, p_fastmode );
            
            triangle.at(0) = current_index + 1;
            triangle.at(1) = current_index + 3;
            triangle.at(2) = current_index + 2;
            p_meshe_dest->AddTriangle( triangle, p_fastmode );            
        }

        int up_skirt_base_index = left_skirt_base_index + left_skirt_nbv;

        for( long i = 0; i < patch_resolution - 1; i++ )
        {
            current_index = i + up_skirt_base_index;

            TrianglePrimitive<unsigned int> triangle;

            triangle.at(0) = current_index;
            triangle.at(1) = current_index + 1;
            triangle.at(2) = current_index + patch_resolution;
            p_meshe_dest->AddTriangle( triangle, p_fastmode );
            
            triangle.at(0) = current_index + 1;
            triangle.at(1) = current_index + 1 + patch_resolution;
            triangle.at(2) = current_index + patch_resolution;
            p_meshe_dest->AddTriangle( triangle, p_fastmode );            
        }


        int down_skirt_base_index = up_skirt_base_index + up_skirt_nbv;

        for( long i = 0; i < patch_resolution - 1; i++ )
        {
            current_index = i + down_skirt_base_index;

            TrianglePrimitive<unsigned int> triangle;

            triangle.at(0) = current_index;
            triangle.at(1) = current_index + 1;
            triangle.at(2) = current_index + patch_resolution;
            p_meshe_dest->AddTriangle( triangle, p_fastmode );
            
            triangle.at(0) = current_index + 1;
            triangle.at(1) = current_index + 1 + patch_resolution;
            triangle.at(2) = current_index + patch_resolution;
            p_meshe_dest->AddTriangle( triangle, p_fastmode );            
        }

    }
}

void Body::BuildMeshes( void )
{
    m_patch_meshe = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );
    build_meshe( cst::patchResolution, m_patch_meshe, false, false );
    m_patch_meshe->SetPath( "sphereLOD patch_meshe" );

    m_patch2_meshe = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );
    build_meshe( cst::patchHighResolution, m_patch2_meshe, true, false );
    m_patch2_meshe->SetPath( "sphereLOD patch2_meshe" );

    m_patch3_meshe = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );
    build_meshe( cst::patchAvgResolution, m_patch3_meshe, true, false );
    m_patch3_meshe->SetPath( "sphereLOD patch3_meshe" );

    m_skirt_meshe = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );
    build_meshe( cst::patchResolution, m_skirt_meshe, false, true );
    m_skirt_meshe->SetPath( "sphereLOD skirt_meshe" );
}

void Body::DestroyMeshes( void )
{
    _DRAWSPACE_DELETE_( m_skirt_meshe );
    _DRAWSPACE_DELETE_(m_patch3_meshe);
    _DRAWSPACE_DELETE_(m_patch2_meshe);
    _DRAWSPACE_DELETE_(m_patch_meshe);
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

void Body::UpdateInvariantViewerPos( const DrawSpace::Utils::Vector& p_pos )
{
    m_invariant_viewerpos = p_pos;
}

void Body::GetInvariantViewerPos( DrawSpace::Utils::Vector& p_pos ) const
{
    p_pos = m_invariant_viewerpos;
}

DrawSpace::Core::Meshe* Body::GetPatcheMeshe( void )
{
    return m_patch_meshe;
}


Patch* Body::GetFaceCurrentLeaf( int p_faceid ) const
{
    QuadtreeNode<Patch>* current_leaf = m_faces[p_faceid]->GetCurrentLeaf();
    return current_leaf->GetContent();
}

dsreal Body::GetHotPointAltitud( void ) const
{
    return m_hotpoint_altitud;
}

int Body::GetCurrentFace (void ) const
{
    return m_current_face;
}
Face* Body::GetFace( int p_faceid ) const
{
    return m_faces[p_faceid];
}

dsreal Body::GetDiameter(void) const
{
    return m_diameter;
}

dsstring Body::GetDescription(void) const
{
    return m_description;
}

dsreal Body::GetRelativeAlt(void) const
{
    return m_relative_alt;
}

void Body::SetHotState( bool p_hotstate )
{
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->SetHotState( p_hotstate );
    }
}

void Body::UpdateRelativeAlt( dsreal p_alt )
{
    m_relative_alt = p_alt;
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->UpdateRelativeAlt( p_alt );
    }
}

void Body::Reset( void )
{
    for( int i = 0; i < 6; i++ )
    {
        m_faces[i]->Reset();
    }
}
