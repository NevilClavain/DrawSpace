/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

#include "proceduralsystem.h"
#include "proceduralaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Systems;
using namespace DrawSpace::Aspect;

ProceduralSystem::ProceduralSystem( void )
{
}

ProceduralSystem::~ProceduralSystem( void )
{
}

void ProceduralSystem::Run( EntityGraph::EntityNodeGraph* p_entitygraph )
{
    m_exec_flag = false;
    p_entitygraph->AcceptSystemRootToLeaf( this );

    // execute procedurals...

    ////////////// TEMPORAIRE, POUR TESTS....

    ///// build...

    ProceduralAspect::RootProceduralBloc* rootpb = new ProceduralAspect::RootProceduralBloc;

    ProceduralAspect::RepeatProceduralBloc* reppb = new ProceduralAspect::RepeatProceduralBloc;

    ProceduralAspect::PublishProceduralBloc* pubpb = new ProceduralAspect::PublishProceduralBloc;

    ProceduralAspect::ValueProceduralBloc<int>* intpb = new ProceduralAspect::ValueProceduralBloc<int>( 5 );

    ProceduralAspect::ValueProceduralBloc<dsstring>* strpb = new ProceduralAspect::ValueProceduralBloc<dsstring>( "megadeth" );

    reppb->m_action = pubpb;
    reppb->m_nbIteration = intpb;

    pubpb->m_proc_pub_evt_handlers = m_proc_pub_evt_handlers;
    pubpb->m_id = "publisher!";

    rootpb->m_children.push_back( reppb );

    m_procedurals["stars generator"] = rootpb;


    ///// execute...

    m_procedurals["stars generator"]->Evaluate();


    ///// cleanup...


    //////////
}

void ProceduralSystem::VisitEntity( Core::Entity* p_parent, Core::Entity* p_entity )
{
    ProceduralAspect* procedural_aspect = p_entity->GetAspect<ProceduralAspect>();
    if( procedural_aspect )
    {
        // temporaire
        
        ComponentList<size_t> operations;
        procedural_aspect->GetComponentsByType<size_t>( operations );

        if( PROCEDURALBLOCID(ProceduralAspect::RootProceduralBloc) == operations[0]->getPurpose() )
        {
            // si true, on va executer une fois cette hierarchie d'entites avec aspect procedural (sous ce ROOT) 
            // jusqu'a rencontrer un prochain root procedural...

            // pour cette raison, eviter d'imbriquer les ROOT dans l'entity Tree ! (pas mettre de ROOT procedural sous un autre ROOT procedural!)
            // chacun chez soi, et les vaches seront bien gardees ;)

            m_exec_flag = procedural_aspect->GetToUpdate();

            // TODO : recup du composant string accompagnant le root, pour cle dans la map m_procedurals qui recevra l'arbre procedural genere
        }
        
        if( m_exec_flag )
        {
            procedural_aspect->Run( p_parent, p_entity );
            procedural_aspect->SetToUpdate( false ); // one shot exec for this root and all its children
        }
    }
}

void ProceduralSystem::RegisterProceduralPublicationEvtHandler( ProceduralPublicationEventHandler* p_handler )
{
    m_proc_pub_evt_handlers.insert( p_handler );
}

void ProceduralSystem::UnregisterProceduralPublicationEvtHandler( ProceduralPublicationEventHandler* p_handler )
{
    m_proc_pub_evt_handlers.erase( p_handler );
}