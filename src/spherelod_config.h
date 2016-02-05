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

#ifndef _SPHERELOD_CONFIG_H_
#define _SPHERELOD_CONFIG_H_

#include "drawspace_commons.h"
#include "spherelod_landscape.h"
#include "spherelod_binder.h"

namespace DrawSpace
{
namespace SphericalLOD
{
class Config
{
public:
    double          m_lod0base;

    Landscape*      m_landscape;

    Binder*         m_mainRenderingBinder;
    Binder*         m_groundCollisionsBinder;
    Binder*         m_patchTexturesBinder;

    Config( void );
    ~Config( void );
};

}
}

#endif
