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


Body::Body( World* p_world, TransformNode* p_drawable ) : 
m_drawable( p_drawable ), 
m_world( p_world )
{
    m_lastworldtrans.Identity();
}

Body::~Body( void )
{
}

btCollisionShape* Body::instanciate_collision_shape( const ShapeDescr& p_shapedescr, btTriangleMesh** p_btmeshe )
{
    switch( p_shapedescr.shape )
    {
        case Body::BOX_SHAPE:
            return _DRAWSPACE_NEW_( btBoxShape, btBoxShape( btVector3( p_shapedescr.box_dims[0], p_shapedescr.box_dims[1], p_shapedescr.box_dims[2] ) ) );
            
        case Body::SPHERE_SHAPE:
            return _DRAWSPACE_NEW_( btSphereShape, btSphereShape( p_shapedescr.sphere_radius ) );

            
        case Body::MESHE_SHAPE:
            {
                btTriangleMesh* data = _DRAWSPACE_NEW_( btTriangleMesh, btTriangleMesh );
                Meshe meshe = p_shapedescr.meshe;

                for( long i = 0; i < meshe.GetTrianglesListSize(); i++ )
                {
                    Triangle curr_triangle;
                    meshe.GetTriangles( i, curr_triangle );

                    Vertex v1, v2, v3;

                    meshe.GetVertex( curr_triangle.vertex1, v1 );
                    meshe.GetVertex( curr_triangle.vertex2, v2 );
                    meshe.GetVertex( curr_triangle.vertex3, v3 );

                    btVector3 a( v1.x, v1.y, v1.z );
                    btVector3 b( v2.x, v2.y, v2.z );
                    btVector3 c( v3.x, v3.y, v3.z );

                    data->addTriangle( a, b, c, false );
                }

                if( p_btmeshe )
                {
                    *p_btmeshe = data;
                }

                return _DRAWSPACE_NEW_( btBvhTriangleMeshShape, btBvhTriangleMeshShape( data, true, true ) );
            } 

        default:
            return NULL;
    }
}

DrawSpace::Core::TransformNode* Body::GetDrawable( void )
{
    return m_drawable;
}

void Body::GetLastWorldTransformation( DrawSpace::Utils::Matrix& p_transfo )
{
    p_transfo = m_lastworldtrans;
}

World* Body::GetWorld( void )
{
    return m_world;
}