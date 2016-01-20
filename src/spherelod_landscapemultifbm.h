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
#include "renderer.h"
#include "plugin.h"

namespace DrawSpace
{
namespace SphericalLOD
{
class LandscapeMultiFbm : public Landscape
{
public:

    dsreal                                  m_mask_seed1;
    dsreal                                  m_mask_seed2;
    dsreal                                  m_mask_input_half_range;

    dsreal                                  m_mountains_lacunarity;
    dsreal                                  m_mountains_roughness;
    dsreal                                  m_mountains_input_half_range;
    dsreal                                  m_mountains_amplitude;
    dsreal                                  m_mountains_seed1;
    dsreal                                  m_mountains_seed2;


    dsreal                                  m_plains_lacunarity;
    dsreal                                  m_plains_roughness;
    dsreal                                  m_plains_input_half_range;
    dsreal                                  m_plains_amplitude;
    dsreal                                  m_plains_seed1;
    dsreal                                  m_plains_seed2;


protected:
   
    DrawSpace::Interface::Renderer*         m_renderer;

public:

    LandscapeMultiFbm( void );
    ~LandscapeMultiFbm( void );

    virtual void InitialiseResources( void );

    virtual void BindShadersParams( void );
    virtual void BindTextures( void );
    virtual void UnbindTextures( void );
};
}
}

#endif
