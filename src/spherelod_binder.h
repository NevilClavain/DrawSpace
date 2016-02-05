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

#ifndef _SPHERELOD_BINDER_H_
#define _SPHERELOD_BINDER_H_

#include "drawspace_commons.h"
#include "fx.h"
#include "texture.h"
#include "renderingnode.h"

namespace DrawSpace
{
namespace SphericalLOD
{

// cette classe peut être vue comme un "descripteur" de renderingNode de planete (SphericalLOD::FaceDrawingNode)
class Binder
{
protected:
    Core::Fx*                                     m_fx;
    Core::Texture*                                m_textures[Core::RenderingNode::NbMaxTextures]; // 32 textures stages max
    Core::Texture*                                m_vertextextures[Core::RenderingNode::NbMaxTextures];

public:

    virtual void Initialise( void ) = 0;

    virtual void Bind( void ) = 0; // appelee juste avant le rendu du node
    virtual void Unbind( void ) = 0; // appelee juste apres le rendu du node

    virtual void SetTexture( Core::Texture* p_texture, long p_stage );
    virtual void SetVertexTexture( Core::Texture* p_texture, long p_stage );
    virtual void SetFx( Core::Fx* p_fx );
    
    virtual Core::Texture* GetTexture( long p_index );
    virtual Core::Texture* GetVertexTexture( long p_index );
    virtual Core::Fx* GetFx( void );
};
}
}

#endif