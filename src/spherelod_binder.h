/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#ifndef _SPHERELOD_BINDER_H_
#define _SPHERELOD_BINDER_H_

#include "drawspace_commons.h"
#include "fx.h"
#include "texture.h"
#include "renderingnode.h"
#include "plugin.h"
#include "renderer.h"

namespace DrawSpace
{
namespace SphericalLOD
{

// cette classe peut �tre vue comme un "descripteur" de renderingNode de planete (SphericalLOD::FaceDrawingNode)
class Binder
{
protected:
    Core::Fx*                                       m_fx;
    Core::Texture*                                  m_textures[Core::RenderingNode::NbMaxTextures]; // 32 textures stages max
    Core::Texture*                                  m_vertextextures[Core::RenderingNode::NbMaxTextures];

    DrawSpace::Interface::Renderer*                 m_renderer;

public:

    Binder( void );

    virtual void Bind( void ) {}; // appelee juste avant le rendu du node
    virtual void Unbind( void ) {}; // appelee juste apres le rendu du node

    void SetRenderer( DrawSpace::Interface::Renderer* p_renderer );

    void SetTexture( Core::Texture* p_texture, long p_stage );
    void SetVertexTexture( Core::Texture* p_texture, long p_stage );
    void SetFx( Core::Fx* p_fx );
    
    Core::Texture* GetTexture( long p_index );
    Core::Texture* GetVertexTexture( long p_index );
    Core::Fx* GetFx( void );
};
}
}

#endif
