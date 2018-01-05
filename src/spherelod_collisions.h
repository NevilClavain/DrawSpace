/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _SPHERELOD_COLLISIONS_H_
#define _SPHERELOD_COLLISIONS_H_

#include "spherelod_subpass.h"

namespace DrawSpace
{

namespace SphericalLOD
{
// fwd declaration
class Layer;

class Collisions : public DrawSpace::SphericalLOD::SubPass
{
protected:

    bool                                    m_enable;

    Layer*                                  m_layer;

    DrawSpace::Core::Texture*               m_collidingheightmap_texture;
    void*                                   m_collidingheightmap_content;

    DrawSpace::IntermediatePass*            m_collidingheightmap_pass;

    DrawSpace::IntermediatePass*            create_colliding_heightmap_pass( void );

public:

    Collisions( Layer* p_owner, DrawSpace::SphericalLOD::Config* p_config, int p_orientation, int p_node_layer_index );
    virtual ~Collisions( void );

    virtual void DrawSubPass( void );
    virtual void SubPassDone( void );
    virtual void Enable( void );
    virtual void Disable( void );
    virtual DrawSpace::Core::Texture* GetHMTexture( void );
    virtual void* GetHMTextureContent( void );

};
}
}

#endif
