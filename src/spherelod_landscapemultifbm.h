/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#ifndef _SPHERELOD_LANDSCAPEMULTIFBM_H_
#define _SPHERELOD_LANDSCAPEMULTIFBM_H_

#include "spherelod_landscape.h"
#include "noise.h"
#include "texture.h"

namespace DrawSpace
{
namespace SphericalLOD
{
class LandscapeMultiFbm : public Landscape
{
protected:

    DrawSpace::Utils::Fractal*          m_fractal;

    void*                               m_pnbufftexture_content;
    void*                               m_pnmaptexture_content;

    DrawSpace::Core::Texture*           m_perlinnoisebuffer_texture;
    DrawSpace::Core::Texture*           m_perlinnoisemap_texture;


public:
    LandscapeMultiFbm( DrawSpace::Utils::Fractal* p_fractal );
    ~LandscapeMultiFbm( void );

    virtual void InitialiseResources( void );

    virtual void BindShadersParams( void );
    virtual void BindTextures( void );
};
}
}

#endif
