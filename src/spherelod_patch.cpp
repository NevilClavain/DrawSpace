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

Patch::Patch( dsreal p_ray, int p_orientation, const dsstring& p_name, Patch* p_parent, int p_parentnodeid, BaseQuadtreeNode* p_owner ) : 
m_orientation( p_orientation ),
m_name( p_name ),
m_ray( p_ray ),
m_owner( p_owner )
{
    for( long i = 0; i < 8; i++ )
    {
        m_neighbours[i] = NULL;
    }

    for( long i = 0; i < Maps::NB_TEXTURETYPE; i++ )
    {
        m_textures_data[i] = NULL;
    }

    if( NULL == p_parent )
    {
        m_xpos = m_ypos = 0.0;
        m_sidelength = 2.0;    // on travaille sur une sphere de rayon = 1.0, donc diametre = 2.0
    }
    else
    {
        m_sidelength = p_parent->m_sidelength / 2.0;

        switch( p_parentnodeid )
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

    // build vsphere characteristics

    Vector center( 0.0, 0.0, 0.0, 1.0 );
    Vector center_out;
    ConvertVertex( center, p_orientation, m_sidelength, p_ray, m_xpos, m_ypos, center_out );

    m_vsphere = _DRAWSPACE_NEW_( DrawSpace::Core::VSphere, DrawSpace::Core::VSphere( center_out, 1.41 * m_sidelength * m_ray / 2.0 ) );

}

Patch::~Patch( void )
{
    _DRAWSPACE_DELETE_( m_vsphere );
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

dsreal Patch::GetTriangleSideLength( void )
{
    return ( ( m_sidelength * m_ray ) / ( Patch::Resolution - 1 ) );
}

void Patch::GetPos( dsreal& p_xpos, dsreal& p_ypos )
{
    p_xpos = m_xpos * m_ray;
    p_ypos = m_ypos * m_ray;
}

BaseQuadtreeNode* Patch::GetOwner( void )
{
    return m_owner;
}

int Patch::GetOrientation( void )
{
    return m_orientation;
}

DrawSpace::Core::VSphere* Patch::GetVSphere( void )
{
    return m_vsphere;
}

void Patch::ConvertVertex( const DrawSpace::Utils::Vector& p_in, int p_orientation, dsreal p_sidelength, dsreal p_ray, dsreal p_posx, dsreal p_posy, DrawSpace::Utils::Vector& p_out )
{
    // effectue la meme transfo que le vertex shader

    DrawSpace::Utils::Vector in = p_in;
    DrawSpace::Utils::Vector v2, v3;

    // sidelenght scaling
    in.Scale( p_sidelength / 2.0 );

    // patch positionning
    in[0] = in[0] + p_posx;
    in[1] = in[1] + p_posy;
    in[2] = 0.0;
    in[3] = 1.0;

    // patch reorientation

    switch( p_orientation )
    {
        case FrontPlanetFace:

            v2[0] = in[0];
            v2[1] = in[1];
            v2[2] = 1.0;
            break;

        case RearPlanetFace:

            v2[0] = -in[0];
            v2[1] = in[1];
            v2[2] = -1.0;
            break;

        case LeftPlanetFace:

            v2[0] = -1.0;
            v2[1] = in[1];
            v2[2] = in[0];
            break;

        case RightPlanetFace:

            v2[0] = 1.0;
            v2[1] = in[1];
            v2[2] = -in[0];
            break;

        case TopPlanetFace:

            v2[0] = in[0];
            v2[1] = 1.0;
            v2[2] = -in[1];
            break;

        case BottomPlanetFace:

            v2[0] = in[0];
            v2[1] = -1.0;
            v2[2] = in[1];
            break;
    }

    v2[3] = 1.0;

    CubeToSphere( v2, v3 );

    // final scaling
    v3.Scale( p_ray );
    v3[3] = 1.0;

    p_out = v3;
}

void Patch::SetTexture( Maps::TextureType p_type, void* p_texturedata )
{
    m_textures_data[p_type] = p_texturedata;
}

void* Patch::GetTexture( Maps::TextureType p_type )
{
    return m_textures_data[p_type];
}
