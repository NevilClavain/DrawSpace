/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#include "entitynodegraph.h"
#include "systems.h"
#include "timeaspect.h"
#include "physicsaspect.h"



using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Aspect;

EntityNodeGraph::EntityNodeGraph( void ) :
m_curr_entity_camera( NULL ),
m_dump_me( false ),
m_root_entity( NULL )
{
}

EntityNodeGraph::~EntityNodeGraph( void )
{
}

EntityNode EntityNodeGraph::SetRoot( Entity* p_entity )
{
    p_entity->OnAddedInGraph( this, m_root_entity );

    m_root_entity = p_entity;
	m_tree.insert( m_root_entity );
	EntityNode node( &m_tree.root(), this );
	return node;
}


void EntityNodeGraph::Erase(void)
{
    m_tree.root().data()->OnRemovedFromGraph( this, m_root_entity );
	m_tree.root().erase();
}

void EntityNodeGraph::AcceptSystemLeafToRoot( Interface::System* p_system )
{
    for( auto it = m_tree.df_post_begin(); it != m_tree.df_post_end(); ++it )
    {
        if( it->is_root() )
        {
            p_system->VisitEntity( NULL, it->data() );
        }
        else
        {
            p_system->VisitEntity( it->parent().data(), it->data() );
        }
    } 
}

void EntityNodeGraph::AcceptSystemRootToLeaf( Interface::System* p_system )
{
    for( auto it = m_tree.df_pre_begin(); it != m_tree.df_pre_end(); ++it )
    {
        if( it->is_root() )
        {
            p_system->VisitEntity( NULL, it->data() );
        }
        else
        {
            p_system->VisitEntity( it->parent().data(), it->data() );
        }
    }
}

void EntityNodeGraph::RegisterNodesEvtHandler( EntityNode::EventsHandler* p_handler )
{
    m_nodesevt_handlers.insert( p_handler );
    // balancer la liste des nodes deja enregistres
    for( auto it = m_tree.begin(); it != m_tree.end(); ++it )
    {
        (*p_handler)( EntityNode::ADDED_IN_TREE, it->data() );
    }
}

void EntityNodeGraph::RegisterCameraEvtHandler( CameraEventHandler* p_handler )
{
    m_camevt_handlers.insert( p_handler );

    // annoncer la camera active au nouvel abonne de cet evt...
    if( m_curr_entity_camera )
    {
        (*p_handler)( CAMERA_ACTIVE, m_curr_entity_camera );
    }
}

void EntityNodeGraph::UnregisterNodesEvtHandler( EntityNode::EventsHandler* p_handler )
{
    m_nodesevt_handlers.erase( p_handler );
}

void EntityNodeGraph::UnregisterCameraEvtHandler( CameraEventHandler* p_handler )
{
    m_camevt_handlers.erase( p_handler );
}

void EntityNodeGraph::GetEntityAncestorsList( Entity* p_entity, std::vector<Entity*>& p_ancestors ) const
{
    if( m_entity_to_node.count( p_entity ) )
    {
        EntityNode::EntityTree::node_type* curr_node = m_entity_to_node.at( p_entity );
    
        while( !curr_node->is_root() )
        {
            curr_node = &curr_node->parent();
            p_ancestors.push_back( curr_node->data() );
        }
    }
}

void EntityNodeGraph::PushSignal_RenderSceneBegin( void )
{
    m_signals.push( SIGNAL_RENDERSCENE_BEGIN );
}

void EntityNodeGraph::PushSignal_RenderSceneEnd( void )
{
    m_signals.push( SIGNAL_RENDERSCENE_END );
}

void EntityNodeGraph::PushSignal_DumpToTrace( void )
{
    m_signals.push( SIGNAL_DUMP_TO_TRACE );
}


void EntityNodeGraph::ProcessSignals( void )
{   
    while( !m_signals.empty() )
    {
        Signals sig = m_signals.front();

        switch( sig )
        {
            case SIGNAL_RENDERSCENE_BEGIN:
            {
                for( auto it = m_tree.begin(); it != m_tree.end(); ++it )
                {
                    Entity* ent = it->data();

                    std::vector<Core::ComponentContainer*> aspects;
                    ent->GetAllAspects( aspects );

                    for( size_t i = 0; i < aspects.size(); ++i )
                    {                    
                        Aspect::TimeAspect* timeAspect = dynamic_cast<Aspect::TimeAspect*>( aspects[i] );
                        if( timeAspect )
                        {
                            timeAspect->Activate();
                        }
                    }
                }            
            }
            break;

            case SIGNAL_RENDERSCENE_END:
            {
                for( auto it = m_tree.begin(); it != m_tree.end(); ++it )
                {
                    Entity* ent = it->data();

                    std::vector<Core::ComponentContainer*> aspects;
                    ent->GetAllAspects( aspects );

                    for( size_t i = 0; i < aspects.size(); ++i )
                    {
                        Aspect::TimeAspect* timeAspect = dynamic_cast<Aspect::TimeAspect*>( aspects[i] );
                        if( timeAspect )
                        {
                            timeAspect->Deactivate();
                        }
                    }
                }
            }
            break;

            case SIGNAL_DUMP_TO_TRACE:
            {
                m_dump_me = true;
            }
            break;
        }
        m_signals.pop();
    }
}

void EntityNodeGraph::SetCurrentCameraEntity( Core::Entity* p_curr_entity_camera )
{
    Entity* prev_camera = m_curr_entity_camera;
    m_curr_entity_camera = p_curr_entity_camera;

    if( prev_camera )
    {
        notify_cam_event( CAMERA_INACTIVE, prev_camera );
    }
    notify_cam_event( CAMERA_ACTIVE, m_curr_entity_camera );
}

Core::Entity* EntityNodeGraph::GetCurrentCameraEntity( void ) const
{
    return m_curr_entity_camera;
}

void EntityNodeGraph::notify_cam_event( CameraEvent p_evt, Entity* p_entity )
{
    for( auto& it = m_camevt_handlers.begin(); it != m_camevt_handlers.end(); ++it )
    {
        ( **it )( p_evt, p_entity );
    }
}

bool EntityNodeGraph::ToDump( void ) const
{
    return m_dump_me;
}

void EntityNodeGraph::ResetDumpFlag( void )
{
    m_dump_me = false;
}

void EntityNodeGraph::OnEntityRemoved(Core::Entity* p_entity)
{
    // event triggered here : an entity is removed

    if (p_entity == m_curr_entity_camera)
    {
        // if removed entity is current camera, set current camera to null
        SetCurrentCameraEntity( nullptr );
    }
}

void EntityNodeGraph::RegisterRigidBody(Core::Entity* p_entity) const
{
    manage_registration_in_physic(p_entity, [](PhysicsAspect* p_aspect, Core::Entity* p_entity) { p_aspect->RegisterRigidBody(p_entity); } );
}

void EntityNodeGraph::RegisterCollider(Core::Entity* p_entity) const
{
    manage_registration_in_physic(p_entity, [](PhysicsAspect* p_aspect, Core::Entity* p_entity) { p_aspect->RegisterCollider(p_entity); });
}

void EntityNodeGraph::manage_registration_in_physic(Core::Entity* p_entity, const std::function<void(PhysicsAspect*, Core::Entity* )>& p_register) const
{
    std::vector<Entity*> ancestors;

    // including the entity itself (if the entity has physics aspect, it is the root of a new bullet physic world)
    ancestors.push_back(p_entity);

    GetEntityAncestorsList(p_entity, ancestors);

    PhysicsAspect* physics_aspect{ nullptr };
    for (auto& e : ancestors)
    {
        PhysicsAspect* curr_physics_aspect{ e->GetAspect<PhysicsAspect>() };

        if (curr_physics_aspect)
        {
            // find first parent with a physic aspect
            physics_aspect = curr_physics_aspect;
            break;
        }
    }

    if (physics_aspect)
    {
        p_register(physics_aspect, p_entity);
    }
    else
    {
        _DSEXCEPTION("Physic aspect required in at least one entity ancestor");
    }
}

void EntityNodeGraph::UnregisterRigidBody(Core::Entity* p_entity) const
{
    manage_registration_in_physic(p_entity, [](PhysicsAspect* p_aspect, Core::Entity* p_entity) { p_aspect->UnregisterRigidBody(p_entity); });
}

void EntityNodeGraph::UnregisterCollider(Core::Entity* p_entity) const
{
    manage_registration_in_physic(p_entity, [](PhysicsAspect* p_aspect, Core::Entity* p_entity) { p_aspect->UnregisterCollider(p_entity); });
}
