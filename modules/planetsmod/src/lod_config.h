/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#pragma once


#include "drawspace_commons.h"

namespace LOD
{
struct Binder;

struct Config
{
public:

    using LayerDescriptor = struct
    {
        bool        enable_collisions;
        bool        enable_datatextures;
        bool        enable_lod;
        long        min_lodlevel;  // valable si enable_lod == true
        dsreal      ray;

        Binder*     groundCollisionsBinder[6];
        Binder*     patchTexturesBinder[6];

        dsstring    description; // for debug purpose only;
    };

    double                          m_lod0base;
    int                             m_nbLODRanges_freeCameras;
    int                             m_nbLODRanges_inertBodies;

    //std::vector<LayerDescriptor>    m_layers_descr;

    std::map<int,LayerDescriptor>   m_layers_descr;


    bool                            m_landplace_patch;

    Config( void );
    ~Config( void );
};
}

