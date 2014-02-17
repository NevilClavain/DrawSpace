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

#include "body.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


Body::Body( World* p_world, DrawSpace::Interface::Drawable* p_drawable ) : 
m_drawable( p_drawable ), 
m_world( p_world )
{
}

Body::~Body( void )
{
}

btCollisionShape* Body::instanciate_collision_shape( const ShapeDescr& p_shapedescr )
{
    switch( p_shapedescr.shape )
    {
        case Body::BOX_SHAPE:
            return _DRAWSPACE_NEW_( btBoxShape, btBoxShape( btVector3( p_shapedescr.box_dims[0], p_shapedescr.box_dims[1], p_shapedescr.box_dims[2] ) ) );
            
        case Body::SPHERE_SHAPE:
            return _DRAWSPACE_NEW_( btSphereShape, btSphereShape( p_shapedescr.sphere_radius ) );

        default:
            return NULL;
    }
}

DrawSpace::Interface::Drawable* Body::GetDrawable( void )
{
    return m_drawable;
}
