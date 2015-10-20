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

#include "spherelod_patch.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::SphericalLOD;

Patch::Patch( dsreal p_ray, int p_orientation, const dsstring& p_name, Patch* p_parent, int p_nodeid, BaseQuadtreeNode* p_owner, bool p_forceuv, const DrawSpace::Utils::Vector& p_uvcoords ) : 
m_orientation( p_orientation ),
m_name( p_name ),
m_ray( p_ray ),
m_owner( p_owner )
{
    for( long i = 0; i < 8; i++ )
    {
        m_neighbours[i] = NULL;
    }

    /*
    for( long i = 0; i < Maps::NB_TEXTURETYPE; i++ )
    {
        m_textures_data[i] = NULL;
    }
    */

    if( NULL == p_parent )
    {
        m_xpos = m_ypos = 0.0;
        m_sidelength = 2.0;    // on travaille sur une sphere de rayon = 1.0, donc diametre = 2.0
    }
    else
    {
        m_sidelength = p_parent->m_sidelength / 2.0;

        switch( p_nodeid )
        {
            case BaseQuadtreeNode::NorthWestNode:

                m_xpos = -p_parent->m_sidelength / 4.0;
                m_xpos += p_parent->m_xpos;
                m_ypos = p_parent->m_sidelength / 4.0;
                m_ypos += p_parent->m_ypos;
                break;

            case BaseQuadtreeNode::NorthEastNode:

                m_xpos = p_parent->m_sidelength / 4.0;
                m_xpos += p_parent->m_xpos;
                m_ypos = p_parent->m_sidelength / 4.0;
                m_ypos += p_parent->m_ypos;
                break;

            case BaseQuadtreeNode::SouthEastNode:

                m_xpos = p_parent->m_sidelength / 4.0;
                m_xpos += p_parent->m_xpos;
                m_ypos = -p_parent->m_sidelength / 4.0;
                m_ypos += p_parent->m_ypos;
                break;

            case BaseQuadtreeNode::SouthWestNode:

                m_xpos = -p_parent->m_sidelength / 4.0;
                m_xpos += p_parent->m_xpos;
                m_ypos = -p_parent->m_sidelength / 4.0;
                m_ypos += p_parent->m_ypos;
                break;

            default:
                m_xpos = 0.0; m_ypos = 0.0;
                break;
        }       
    }

    if( p_forceuv )
    {
        m_u1 = p_uvcoords[0];
        m_v1 = p_uvcoords[1];
        m_u2 = p_uvcoords[2];
        m_v2 = p_uvcoords[3];
    }
    else
    {
        if( p_parent )
        {
            dsreal ui, vi;
           
            dsreal x1, y1, x2, y2;

            // calcul pour coin superieur gauche (u1, v1)

            x1 = m_xpos - m_sidelength / 2.0;
            y1 = m_ypos + m_sidelength / 2.0;

            // passer du repere [- p_parent->m_sidelength / 2.0 ; + p_parent->m_sidelength / 2.0] au repere [0.0 ; +1.0] (coords texture uv)
            ui = x1 + ( p_parent->m_sidelength / 2.0 );
            vi = ( p_parent->m_sidelength / 2.0 ) - y1;

            // ajuster pour prendre en compte les coords uv du parent
            m_u1 = ( ui * ( p_parent->m_u2 - p_parent->m_u1 ) ) + p_parent->m_u1;
            m_v1 = ( vi * ( p_parent->m_v2 - p_parent->m_v1 ) ) + p_parent->m_v1;


            // calcul pour coin inferieur droit (u2, v2)

            x2 = m_xpos + m_sidelength / 2.0;
            y2 = m_ypos - m_sidelength / 2.0;

            // passer du repere [- p_parent->m_sidelength / 2.0 ; + p_parent->m_sidelength / 2.0] au repere [0.0 ; +1.0] (coords texture uv)
            ui = x2 + ( p_parent->m_sidelength / 2.0 );
            vi = ( p_parent->m_sidelength / 2.0 ) - y2;

            // ajuster pour prendre en compte les coords uv du parent
            m_u2 = ( ui * ( p_parent->m_u2 - p_parent->m_u1 ) ) + p_parent->m_u1;
            m_v2 = ( vi * ( p_parent->m_v2 - p_parent->m_v1 ) ) + p_parent->m_v1;

        }
        else
        {
            m_u1 = 0.0;
            m_v1 = 0.0;
            m_u2 = 1.0;
            m_v2 = 1.0;
        }
    }
}

Patch::~Patch( void )
{
}

void Patch::SetNeighbour( DrawSpace::Utils::BaseQuadtreeNode* p_patch, int p_id )
{
    m_neighbours[p_id] = p_patch;
}

DrawSpace::Utils::BaseQuadtreeNode* Patch::GetNeighbour( int p_id )
{
    return m_neighbours[p_id];
}

void Patch::CubeToSphere( const Vector& p_in, Vector& p_out )
{
    dsreal x = p_in[0];
    dsreal y = p_in[1];
    dsreal z = p_in[2];

    p_out[0] = x * sqrt( 1.0 - y * y * 0.5 - z * z * 0.5 + y * y * z * z / 3.0 );
    p_out[1] = y * sqrt( 1.0 - z * z * 0.5 - x * x * 0.5 + x * x * z * z / 3.0 );
    p_out[2] = z * sqrt( 1.0 - x * x * 0.5 - y * y * 0.5 + x * x * y * y / 3.0 );
}

void Patch::SphereToCube( const Vector& p_in, Vector& p_out )
{
    dsreal nx, ny, nz;
    nx = ny = nz = 0;

    dsreal x, y, z;

    x = p_in[0];
    y = p_in[1];
    z = p_in[2];

    dsreal fx, fy, fz;
    fx = fabs( p_in[0] );
    fy = fabs( p_in[1] );
    fz = fabs( p_in[2] );

    int nbIter = 6;

    if( fy >= fx && fy >= fz )
    {
        if( y > 0 )
        {
            for( int n = 0; n < nbIter; n++ )
            {
                nx = x / sqrt( 1.0 - ny * ny * 0.5 - nz * nz * 0.5 + ny * ny * nz * nz / 3.0 );
                ny = y / sqrt( 1.0 - nz * nz * 0.5 - nx * nx * 0.5 + nz * nz * nx * nx / 3.0 );
                nz = z / sqrt( 1.0 - nx * nx * 0.5 - ny * ny * 0.5 + nx * nx * ny * ny / 3.0 );

                nx = nx / ny;
                nz = nz / ny;
                ny = 1.0;
            }
        }
        else
        {
            for( int n = 0; n < nbIter; n++ )
            {
                nx = x / sqrt( 1.0 - ny * ny * 0.5 - nz * nz * 0.5 + ny * ny * nz * nz / 3.0 );
                ny = y / sqrt( 1.0 - nz * nz * 0.5 - nx * nx * 0.5 + nz * nz * nx * nx / 3.0 );
                nz = z / sqrt( 1.0 - nx * nx * 0.5 - ny * ny * 0.5 + nx * nx * ny * ny / 3.0 );

                nx = nx / (-ny);
                nz = nz / (-ny);
                ny = -1.0;
            }
        }
    }
    else if( fx >= fy && fx >= fz )
    {
        if( x > 0 )
        {
            for( int n = 0; n < nbIter; n++ )
            {
                nx = x / sqrt( 1.0 - ny * ny * 0.5 - nz * nz * 0.5 + ny * ny * nz * nz / 3.0 );
                ny = y / sqrt( 1.0 - nz * nz * 0.5 - nx * nx * 0.5 + nz * nz * nx * nx / 3.0 );
                nz = z / sqrt( 1.0 - nx * nx * 0.5 - ny * ny * 0.5 + nx * nx * ny * ny / 3.0 );

                nz = nz / nx;
                ny = ny / nx;
                nx = 1.0;
            }
        }
        else
        {
            for( int n = 0; n < nbIter; n++ )
            {
                nx = x / sqrt( 1.0 - ny * ny * 0.5 - nz * nz * 0.5 + ny * ny * nz * nz / 3.0 );
                ny = y / sqrt( 1.0 - nz * nz * 0.5 - nx * nx * 0.5 + nz * nz * nx * nx / 3.0 );
                nz = z / sqrt( 1.0 - nx * nx * 0.5 - ny * ny * 0.5 + nx * nx * ny * ny / 3.0 );

                nz = nz / (-nx);
                ny = ny / (-nx);
                nx = -1.0;
            }
        }
    }
    else
    {
        if( z > 0 )
        {
            for( int n = 0; n < nbIter; n++ )
            {
                nx = x / sqrt( 1.0 - ny * ny * 0.5 - nz * nz * 0.5 + ny * ny * nz * nz / 3.0 );
                ny = y / sqrt( 1.0 - nz * nz * 0.5 - nx * nx * 0.5 + nz * nz * nx * nx / 3.0 );
                nz = z / sqrt( 1.0 - nx * nx * 0.5 - ny * ny * 0.5 + nx * nx * ny * ny / 3.0 );

                nx = nx / nz;
                ny = ny / nz;
                nz = 1.0;
            }
        }
        else
        {
            for( int n = 0; n < nbIter; n++ )
            {
                nx = x / sqrt( 1.0 - ny * ny * 0.5 - nz * nz * 0.5 + ny * ny * nz * nz / 3.0 );
                ny = y / sqrt( 1.0 - nz * nz * 0.5 - nx * nx * 0.5 + nz * nz * nx * nx / 3.0 );
                nz = z / sqrt( 1.0 - nx * nx * 0.5 - ny * ny * 0.5 + nx * nx * ny * ny / 3.0 );

                nx = nx / (-nz);
                ny = ny / (-nz);
                nz = -1.0;
            }
        }
    }

    p_out[0] = nx;
    p_out[1] = ny;
    p_out[2] = nz;
}


void Patch::GetName( dsstring& p_name )
{
    p_name = m_name;
}

dsreal Patch::GetSideLength( void )
{
    return m_sidelength * m_ray;
}

dsreal Patch::GetUnitSideLenght( void )
{
    return m_sidelength;
}

dsreal Patch::GetTriangleSideLength( void )
{
    return ( ( m_sidelength * m_ray ) / ( Patch::Resolution - 1 ) );
}

void Patch::GetPos( dsreal& p_xpos, dsreal& p_ypos )
{
    p_xpos = m_xpos * m_ray;
    p_ypos = m_ypos * m_ray;
}

void Patch::GetUnitPos( dsreal& p_xpos, dsreal& p_ypos )
{
    p_xpos = m_xpos;
    p_ypos = m_ypos;
}

BaseQuadtreeNode* Patch::GetOwner( void )
{
    return m_owner;
}

int Patch::GetOrientation( void )
{
    return m_orientation;
}

void Patch::XYToXYZ( int p_orientation, dsreal p_x, dsreal p_y, Vector& p_out )
{
    switch( p_orientation )
    {
        case SphericalLOD::Patch::FrontPlanetFace:

            p_out[0] = p_x;
            p_out[1] = p_y;
            p_out[2] = 1.0;
            break;

        case SphericalLOD::Patch::RearPlanetFace:

            p_out[0] = -p_x;
            p_out[1] = p_y;
            p_out[2] = -1.0;
            break;

        case SphericalLOD::Patch::LeftPlanetFace:

            p_out[0] = -1.0;
            p_out[1] = p_y;
            p_out[2] = p_x;
            break;

        case SphericalLOD::Patch::RightPlanetFace:

            p_out[0] = 1.0;
            p_out[1] = p_y;
            p_out[2] = -p_x;
            break;

        case SphericalLOD::Patch::TopPlanetFace:

            p_out[0] = p_x;
            p_out[1] = 1.0;
            p_out[2] = -p_y;
            break;

        case SphericalLOD::Patch::BottomPlanetFace:

            p_out[0] = p_x;
            p_out[1] = -1.0;
            p_out[2] = p_y;
            break;
    }
}

void Patch::ProjectVertex( const DrawSpace::Utils::Vector& p_in, DrawSpace::Utils::Vector& p_out )
{
    DrawSpace::Utils::Vector in = p_in;
    DrawSpace::Utils::Vector v2, v3;

    // sidelenght scaling
    in.Scale( m_sidelength / 2.0 );

    // patch positionning
    in[0] = in[0] + m_xpos;
    in[1] = in[1] + m_ypos;
    in[2] = 0.0;
    in[3] = 1.0;

    // patch reorientation
    XYToXYZ( m_orientation, in[0], in[1], v2 );
    v2[3] = 1.0;

    CubeToSphere( v2, v3 );
    v3[3] = 1.0;
    p_out = v3;
}

/*
void Patch::SetTexture( Maps::TextureType p_type, void* p_texturedata )
{
    m_textures_data[p_type] = p_texturedata;
}

void* Patch::GetTexture( Maps::TextureType p_type )
{
    return m_textures_data[p_type];
}
*/
