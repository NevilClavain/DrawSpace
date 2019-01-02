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

#ifndef _ENTITYNODEGRAPH_H_
#define _ENTITYNODEGRAPH_H_

#include "entitynode.h"
#include <queue>

namespace DrawSpace
{
namespace Interface
{
class System;
}

namespace EntityGraph
{
class EntityNodeGraph sealed
{
public:
	using EntityTree = st_tree::tree<Core::Entity*>;

    typedef enum
    {
        CAMERA_ACTIVE,
        CAMERA_INACTIVE

    } CameraEvent;

    typedef DrawSpace::Core::BaseCallback2<void, CameraEvent, Core::Entity*>                            CameraEventHandler;

private:

    typedef enum
    {
        SIGNAL_RENDERSCENE_BEGIN,
        SIGNAL_RENDERSCENE_END,
        SIGNAL_DUMP_TO_TRACE

    } Signals;

	mutable EntityTree                                          m_tree;

    std::set<EntityNode::EventsHandler*>                        m_nodesevt_handlers;
    std::set<CameraEventHandler*>                               m_camevt_handlers;

    std::map<Core::Entity*, EntityNode::EntityTree::node_type*> m_entity_to_node;

    Core::Entity*                                               m_curr_entity_camera;

    std::queue<Signals>                                         m_signals;

    bool                                                        m_dump_me;

    Core::Entity*                                               m_root_entity; // on memorise le root juste comme ca, ca peut servir

    void notify_cam_event( CameraEvent p_evt, Core::Entity* p_entity );

public:
	EntityNodeGraph(void);
	~EntityNodeGraph(void);

	EntityNode SetRoot( Core::Entity* p_entity );
	void Erase( void );
    
    void AcceptSystemLeafToRoot( DrawSpace::Interface::System* p_system );
    void AcceptSystemRootToLeaf( DrawSpace::Interface::System* p_system );

    void RegisterNodesEvtHandler( EntityNode::EventsHandler* p_handler );
    void RegisterCameraEvtHandler( CameraEventHandler* p_handler );

    void UnregisterNodesEvtHandler( EntityNode::EventsHandler* p_handler );
    void UnregisterCameraEvtHandler( CameraEventHandler* p_handler );


    void GetEntityAncestorsList( Core::Entity* p_entity, std::vector<Core::Entity*>& p_ancestors ) const;

    void PushSignal_RenderSceneBegin( void );
    void PushSignal_RenderSceneEnd( void );
    void PushSignal_DumpToTrace( void );

    void ProcessSignals( void );

    void SetCurrentCameraEntity( Core::Entity* p_curr_entity_camera );
    Core::Entity* GetCurrentCameraEntity( void ) const;

    bool ToDump( void ) const;
    void ResetDumpFlag( void );
    
    friend class EntityNode;
};
}
}

#endif
