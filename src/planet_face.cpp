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

#include "planet_face.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Planet;
using namespace DrawSpace::Utils;


Face::Face( void )
{
    m_rootpatch = _DRAWSPACE_NEW_( QuadtreeNode<Patch>, QuadtreeNode<Patch>( m_patchleafs ) );
}

Face::~Face( void )
{
    _DRAWSPACE_DELETE_( m_rootpatch );
}

void Face::Draw( const DrawSpace::Utils::Matrix& p_world, DrawSpace::Utils::Matrix& p_view )
{
    for( std::map<Utils::BaseQuadtreeNode*, Utils::BaseQuadtreeNode*>::iterator it = m_patchleafs.begin(); it != m_patchleafs.end(); ++it )
    {
        // rendu du patch leaf
    }
}