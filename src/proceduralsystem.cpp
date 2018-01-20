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
    //std::unordered_map<dsstring,std::vector<Aspect::ProceduralAspect::ProceduralBloc*>> procedurals_blocs;
    //std::unordered_map<dsstring, Aspect::ProceduralAspect::RootProceduralBloc*>         procedurals_tree;

    m_exec_flag = false;
    p_entitygraph->AcceptSystemRootToLeaf( this );

    // execute procedurals...

    ////////////// TEMPORAIRE, POUR TESTS....

    ///// build...

    ProceduralAspect::ProceduralBlocsFactory factory;

    ProceduralAspect::RootProceduralBloc* rootpb = factory.CreateRootBloc( "stars generator" );

    ProceduralAspect::RepeatProceduralBloc<int>* reppb = factory.CreateBloc<ProceduralAspect::RepeatProceduralBloc<int>>( "stars generator" );
    ProceduralAspect::PublishProceduralBloc* pubpb = factory.CreateBloc<ProceduralAspect::PublishProceduralBloc>( "stars generator" );
    ProceduralAspect::UniformRandomValueProceduralBloc<int>* uripb = factory.CreateBloc<ProceduralAspect::UniformRandomValueProceduralBloc<int>>( "stars generator" );
    ProceduralAspect::SimpleValueProceduralBloc<int>* rminpb = factory.CreateBloc<ProceduralAspect::SimpleValueProceduralBloc<int>>( "stars generator" );
    ProceduralAspect::SimpleValueProceduralBloc<int>* rmaxpb = factory.CreateBloc<ProceduralAspect::SimpleValueProceduralBloc<int>>( "stars generator" );
    ProceduralAspect::SimpleValueProceduralBloc<int>* rseedpb = factory.CreateBloc<ProceduralAspect::SimpleValueProceduralBloc<int>>( "stars generator" );

    rminpb->SetValue( 7 );
    rmaxpb->SetValue( 11 );
    rseedpb->SetValue( 3455776 );

    uripb->m_inf = rminpb;
    uripb->m_sup = rmaxpb;
    uripb->m_seed = rseedpb;

    reppb->m_action = pubpb;
    reppb->m_nbIteration = uripb;

    pubpb->m_proc_pub_evt_handlers = m_proc_pub_evt_handlers;
    pubpb->m_id = "publisher!";

    rootpb->m_children.push_back( reppb );

    ///// execute...

    rootpb->Evaluate();

    factory.CleanTreeBlocs( "stars generator" );    
    


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