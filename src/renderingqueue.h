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

#ifndef _RENDERINGQUEUE_H_
#define _RENDERINGQUEUE_H_

#include "drawspace_commons.h"
#include "renderingnode.h"

namespace DrawSpace
{
namespace Core
{
class RenderingQueue
{
protected:

    std::vector<RenderingNode*>	        m_nodes;
    Texture*                            m_target;
    bool                                m_clear_depth;
    bool                                m_clear_target;
    unsigned char                       m_target_clear_color_r;
    unsigned char                       m_target_clear_color_g;
    unsigned char                       m_target_clear_color_b;

public:
    RenderingQueue( void );
    RenderingQueue( Texture* p_target );
    ~RenderingQueue( void );

    void Add( RenderingNode* p_node );
    void Draw( void );
    
    //void DrawMultiples( void );

    void EnableDepthClearing( bool p_enable );
    void EnableTargetClearing( bool p_enable );
    void SetTargetClearingColor( unsigned char p_r, unsigned char p_g, unsigned char p_b );
};
}
}

#endif