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

#ifndef _SPHERELOD_PATCH_H_
#define _SPHERELOD_PATCH_H_

#include "vector.h"
#include "quadtree.h"
#include "vsphere.h"
#include "spherelod_maps.h"
#include "texture.h"

namespace DrawSpace
{
namespace SphericalLOD
{
class Patch
{
public:

    static const int    Resolution          = 22;//55;

    static const int    NorthNeighbour      = 0;
    static const int    SouthNeighbour      = 1;
    static const int    WestNeighbour       = 2;
    static const int    EastNeighbour       = 3;
    static const int    NorthWestNeighbour  = 4;
    static const int    NorthEastNeighbour  = 5;
    static const int    SouthWestNeighbour  = 6;
    static const int    SouthEastNeighbour  = 7;

    static const int    FrontPlanetFace    = 0;
    static const int    RearPlanetFace     = 1;
    static const int    LeftPlanetFace     = 2;
    static const int    RightPlanetFace    = 3;
    static const int    TopPlanetFace      = 4;
    static const int    BottomPlanetFace   = 5;

protected:

    dsreal                                  m_sidelength;
    dsreal                                  m_xpos;
    dsreal                                  m_ypos;
    dsreal                                  m_ray;

    int                                     m_orientation;

    DrawSpace::Utils::BaseQuadtreeNode*     m_owner;

    DrawSpace::Utils::BaseQuadtreeNode*     m_neighbours[8];
    dsstring		                        m_name;

    DrawSpace::Core::VSphere*               m_vsphere;


    void*                                   m_textures_data[Maps::NB_TEXTURETYPE];

public:
    Patch( dsreal p_ray, int p_orientation, const dsstring& p_name, Patch* p_parent, int p_parentnodeid, DrawSpace::Utils::BaseQuadtreeNode* p_owner );
    virtual ~Patch( void );

    void SetNeighbour( DrawSpace::Utils::BaseQuadtreeNode* p_patch, int p_id );
    DrawSpace::Utils::BaseQuadtreeNode* GetNeighbour( int p_id );

    void GetName( dsstring& p_name );
    dsreal GetSideLength( void );
    int GetOrientation( void );

    dsreal GetTriangleSideLength( void );

    void GetPos( dsreal& p_xpos, dsreal& p_ypos );

    DrawSpace::Utils::BaseQuadtreeNode* GetOwner( void );

    DrawSpace::Core::VSphere* GetVSphere( void );

    void SetTexture( Maps::TextureType p_type, void* p_texturedata );

    void* GetTexture( Maps::TextureType p_type );

    static void CubeToSphere( const DrawSpace::Utils::Vector& p_in, DrawSpace::Utils::Vector& p_out );
    static void SphereToCube( const DrawSpace::Utils::Vector& p_in, DrawSpace::Utils::Vector& p_out );
    
    static void ConvertVertex( const DrawSpace::Utils::Vector& p_in, int p_orientation, dsreal p_sidelength, dsreal p_ray, dsreal p_posx, dsreal p_posy, DrawSpace::Utils::Vector& p_out );

};
}
}

#endif