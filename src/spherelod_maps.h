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

#ifndef _SPHERELOD_MAPS_H_
#define _SPHERELOD_MAPS_H_


#include "vector.h"
#include "texture.h"


namespace DrawSpace
{
namespace SphericalLOD
{
class Patch;

class Maps
{
public:

    typedef enum
    {
        ELEVATION_TEXTURE = 0,
        COLOR_TEXTURE = 1,
        NORMALS_TEXTURE = 2,

        NB_TEXTURETYPE = 3,

    } TextureType;

protected:

    DrawSpace::Core::Texture*   m_elevation_texture;
    DrawSpace::Core::Texture*   m_color_texture;

    void*                       m_elevation_texture_data;
    void*                       m_color_texture_data;


public:
    Maps( void );
    ~Maps( void );


    bool Initialize( void );

    dsreal GetAltFromLocalPoint( const DrawSpace::Utils::Vector& p_point );
    dsreal GetAltFromGlobalPoint( const DrawSpace::Utils::Vector& p_point );

    void* RequestTexture( TextureType p_texturetype, Patch* p_patch );

};
}
}
#endif