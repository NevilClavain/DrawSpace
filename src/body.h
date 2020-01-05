/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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

#ifndef _BODY_H_
#define _BODY_H_


#include "matrix.h"
#include "meshe.h"
#include "world.h"
#include "timemanager.h"

namespace DrawSpace
{
namespace Dynamics
{
class Body
{
public:

    typedef enum
    {
        ATTACHED,
        DETACHED
        
    } Event;

    typedef DrawSpace::Core::BaseCallback2<void, Event, Body*> EventHandler;


    typedef enum
    {
        BOX_SHAPE,
        SPHERE_SHAPE,
        MESHE_SHAPE,

    } Shape;

    typedef struct
    {
        Shape                       shape;
        DrawSpace::Utils::Vector    box_dims;
        dsreal                      sphere_radius;
        DrawSpace::Core::Meshe      meshe;

    } ShapeDescr;

    typedef struct
    {

        DrawSpace::Utils::Matrix    initial_attitude;

        dsreal                      mass;

        Body::ShapeDescr            shape_descr;

    } Parameters;

protected:
    
    typedef enum
    {
        MEMMANAGER_SINGLETON,
        MEMMANAGER_EXTERNAL,

    } MemManagerSource;


    World*                              m_world;
    bool                                m_contact_state;
    DrawSpace::Utils::Matrix            m_lastworldtrans;
    std::vector<EventHandler*>          m_evt_handlers;
    std::map<Body*, Body*>              m_attached_inertbodies;

    MemManagerSource                    m_memmgr_source;
    DrawSpace::Utils::MemAlloc*         m_memmgr;           // si m_memmgr_source == MEMMANAGER_EXTERNAL

    btCollisionShape*                   instanciate_collision_shape( const ShapeDescr& p_shapedescr, btTriangleMesh** p_btmeshe = NULL );

public:

    Body( void );
    Body( World* p_world );
    virtual ~Body( void );

    void GetLastWorldTransformation( DrawSpace::Utils::Matrix& p_transfo );
    World* GetWorld( void );

    virtual btRigidBody* GetRigidBody( void ) = 0;

    virtual bool GetContactState( void );
    virtual void SetContactState( bool p_state );

    virtual void RegisterAttachedInertBody( Body* p_body );
    virtual void UnregisterAttachedInertBody( Body* p_body );

    virtual void Update2( DrawSpace::Utils::TimeManager& p_timemanager );

    virtual void SetExternalMemManagerSource( DrawSpace::Utils::MemAlloc* p_memmgr );
};
}
}
#endif
