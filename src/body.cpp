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

#include "body.h"
#include "inertbody.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


Body::Body( void ) :
m_world( NULL ),
m_contact_state( false ),
m_memmgr_source( MEMMANAGER_SINGLETON )
{
    m_lastworldtrans.Identity();
}


Body::Body( World* p_world ) :
m_world( p_world ),
m_contact_state( false ),
m_memmgr_source( MEMMANAGER_SINGLETON )
{
    m_lastworldtrans.Identity();
}

Body::~Body( void )
{
}

void Body::SetExternalMemManagerSource( DrawSpace::Utils::MemAlloc* p_memmgr )
{
    m_memmgr_source = MEMMANAGER_EXTERNAL;
    m_memmgr = p_memmgr;
}

btCollisionShape* Body::instanciate_collision_shape( const ShapeDescr& p_shapedescr, btTriangleMesh** p_btmeshe )
{
    dsreal world_scale = World::m_scale;

    switch( p_shapedescr.shape )
    {
        case Body::BOX_SHAPE:
            {
                if( MEMMANAGER_SINGLETON == m_memmgr_source )
                {
                    return _DRAWSPACE_NEW_( btBoxShape, btBoxShape( btVector3( p_shapedescr.box_dims[0] * world_scale, p_shapedescr.box_dims[1] * world_scale, p_shapedescr.box_dims[2] * world_scale ) ) );
                }
                else
                {
                    return _DRAWSPACE_NEW_FROM_MEMMGR_( m_memmgr, btBoxShape, btBoxShape( btVector3( p_shapedescr.box_dims[0] * world_scale, p_shapedescr.box_dims[1] * world_scale, p_shapedescr.box_dims[2] * world_scale ) ) );
                }
            }
            
        case Body::SPHERE_SHAPE:
            {
                if( MEMMANAGER_SINGLETON == m_memmgr_source )
                {
                    return _DRAWSPACE_NEW_( btSphereShape, btSphereShape( p_shapedescr.sphere_radius * world_scale ) );
                }
                else
                {
                    return _DRAWSPACE_NEW_FROM_MEMMGR_( m_memmgr, btSphereShape, btSphereShape( p_shapedescr.sphere_radius * world_scale ) );
                }
            }
            
        case Body::MESHE_SHAPE:
            {
                btTriangleMesh* data;
                
                if( MEMMANAGER_SINGLETON == m_memmgr_source )
                {
                    data = _DRAWSPACE_NEW_( btTriangleMesh, btTriangleMesh );
                }
                else
                {
                    data = _DRAWSPACE_NEW_FROM_MEMMGR_( m_memmgr, btTriangleMesh, btTriangleMesh );
                }

                Meshe meshe = p_shapedescr.meshe;

                for( long i = 0; i < meshe.GetTrianglesListSize(); i++ )
                {
                    Triangle curr_triangle;
                    meshe.GetTriangles( i, curr_triangle );

                    Vertex v1, v2, v3;

                    meshe.GetVertex( curr_triangle.vertex1, v1 );
                    meshe.GetVertex( curr_triangle.vertex2, v2 );
                    meshe.GetVertex( curr_triangle.vertex3, v3 );

                    btVector3 a( v1.x * world_scale, v1.y * world_scale, v1.z * world_scale );
                    btVector3 b( v2.x * world_scale, v2.y * world_scale, v2.z * world_scale );
                    btVector3 c( v3.x * world_scale, v3.y * world_scale, v3.z * world_scale );

                    data->addTriangle( a, b, c, false );
                }

                if( p_btmeshe )
                {
                    *p_btmeshe = data;
                }

                if( MEMMANAGER_SINGLETON == m_memmgr_source )
                {
                    return _DRAWSPACE_NEW_( btBvhTriangleMeshShape, btBvhTriangleMeshShape( data, true, true ) );
                }
                else
                {
                    return _DRAWSPACE_NEW_FROM_MEMMGR_( m_memmgr, btBvhTriangleMeshShape, btBvhTriangleMeshShape( data, true, true ) );
                }
            } 

        default:
            return NULL;
    }
}
/*
*/

void Body::GetLastWorldTransformation( DrawSpace::Utils::Matrix& p_transfo )
{
    p_transfo = m_lastworldtrans;
}

World* Body::GetWorld( void )
{
    return m_world;
}

bool Body::GetContactState( void )
{
    return m_contact_state;
}

void Body::SetContactState( bool p_state )
{
    m_contact_state = p_state;
}

void Body::RegisterAttachedInertBody( Body* p_body )
{
    m_attached_inertbodies[p_body] = p_body;

    
}

void Body::UnregisterAttachedInertBody( Body* p_body )
{
    if( m_attached_inertbodies.count( p_body ) > 0 )
    {
        m_attached_inertbodies.erase( p_body );        
    }
}

void Body::Update2( DrawSpace::Utils::TimeManager& p_timemanager )
{
    // inertbodies attaches a ce body : on deporte l'execution de leur transfo ici, dans Update2 du body
    // auquel ils sont attache, plutot que dans SceneNode::ComputeTransformation() des inertbodies
    // et ce afin d'etre sur que les transfos de ces inertbodies sont toujours � jour par rapport
    // a la transfo du body auquel ils sont attach�s

    for( std::map<Body*, Body*>::iterator it = m_attached_inertbodies.begin(); it != m_attached_inertbodies.end(); ++it )
    {
        InertBody* inertbody = dynamic_cast<InertBody*>( it->first );
        if( inertbody )
        {
            //inertbody->UpdateAsAttached( p_timemanager );
           
            BaseSceneNode* node = inertbody->GetOwner();
            node->ForceComputeTransformation( p_timemanager );
        }
    }
}
