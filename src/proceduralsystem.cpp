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
    ProceduralAspect::SeedSourceProceduralBloc::Initialize( 123 );
}

ProceduralSystem::~ProceduralSystem( void )
{
}

bool ProceduralSystem::Init( EntityGraph::EntityNodeGraph* p_entitygraph )
{
    return true;
}

void ProceduralSystem::Release( EntityGraph::EntityNodeGraph* p_entitygraph )
{

}


void ProceduralSystem::Run( EntityGraph::EntityNodeGraph* p_entitygraph )
{
    m_exec_flag = false;
    p_entitygraph->AcceptSystemRootToLeaf( this );

    // execute procedurals...

    ////////////// TEMPORAIRE, POUR TESTS....

    ///// build...

    /*
    ProceduralAspect::ProceduralBlocsFactory factory;

    ProceduralAspect::RootProceduralBloc* rootpb = factory.CreateRootBloc( "stars generator" );

    ProceduralAspect::RepeatProceduralBloc<int>* reppb = factory.CreateBloc<ProceduralAspect::RepeatProceduralBloc<int>>( "stars generator" );

    ProceduralAspect::PublishProceduralBloc* pubpb = factory.CreateBloc<ProceduralAspect::PublishProceduralBloc>( "stars generator" );
    ProceduralAspect::PublishProceduralBloc* pubpb2 = factory.CreateBloc<ProceduralAspect::PublishProceduralBloc>( "star x value" );


    ProceduralAspect::UniformRandomValueProceduralBloc<int>* uripb = factory.CreateBloc<ProceduralAspect::UniformRandomValueProceduralBloc<int>>( "stars generator" );
    ProceduralAspect::UniformRandomValueProceduralBloc<int>* uripb2 = factory.CreateBloc<ProceduralAspect::UniformRandomValueProceduralBloc<int>>( "stars generator" );
    ProceduralAspect::SimpleValueProceduralBloc<int>* rminpb = factory.CreateBloc<ProceduralAspect::SimpleValueProceduralBloc<int>>( "stars generator", 7 );
    ProceduralAspect::SimpleValueProceduralBloc<int>* rmaxpb = factory.CreateBloc<ProceduralAspect::SimpleValueProceduralBloc<int>>( "stars generator", 11 );

    ProceduralAspect::SimpleValueProceduralBloc<int>* rminpb2 = factory.CreateBloc<ProceduralAspect::SimpleValueProceduralBloc<int>>( "stars generator", 0 );
    ProceduralAspect::SimpleValueProceduralBloc<int>* rmaxpb2 = factory.CreateBloc<ProceduralAspect::SimpleValueProceduralBloc<int>>( "stars generator", 5 );


    ProceduralAspect::SeedSourceProceduralBloc* seedsrc = factory.CreateBloc<ProceduralAspect::SeedSourceProceduralBloc>( "stars generator" );
    ProceduralAspect::SeedSourceProceduralBloc* seedsrc2 = factory.CreateBloc<ProceduralAspect::SeedSourceProceduralBloc>( "stars generator" );

    ProceduralAspect::ArrayProceduralBloc<dsreal>* apb = factory.CreateBloc<ProceduralAspect::ArrayProceduralBloc<dsreal>>( "stars generator" );
    ProceduralAspect::SimpleValueProceduralBloc<int>* index1 = factory.CreateBloc<ProceduralAspect::SimpleValueProceduralBloc<int>>( "stars generator", 2 );

    ProceduralAspect::ArrayProceduralBloc<dsstring>* apb2 = factory.CreateBloc<ProceduralAspect::ArrayProceduralBloc<dsstring>>( "stars generator" );
    

    apb->PushValue( 0.1 );
    apb->PushValue( 0.2 );
    apb->PushValue( 0.3 );
    apb->m_args.push_back( index1 );

    apb2->PushValue( "alpha" );
    apb2->PushValue( "beta" );
    apb2->PushValue( "delta" );
    apb2->PushValue( "echo" );
    apb2->PushValue( "foxtrot" );
    apb2->PushValue( "gamma" );
    apb2->m_args.push_back( uripb2 );
    
    uripb2->m_args.push_back( seedsrc2 );
    uripb2->m_args.push_back( rmaxpb2 );
    uripb2->m_args.push_back( rminpb2 );

    uripb->m_args.push_back( seedsrc );
    uripb->m_args.push_back( rmaxpb );
    uripb->m_args.push_back( rminpb );


    reppb->m_args.push_back( uripb );
    reppb->m_args.push_back( pubpb );
    reppb->m_args.push_back( pubpb2 );



    pubpb->m_proc_pub_evt_handlers = m_proc_pub_evt_handlers;
    pubpb->m_id = "publisher!";
    pubpb->m_args.push_back( apb );


    pubpb2->m_proc_pub_evt_handlers = m_proc_pub_evt_handlers;
    pubpb2->m_id = "x value!";

    pubpb2->m_args.push_back( apb2 );

    rootpb->m_args.push_back( reppb );

    ///// execute...

    rootpb->Evaluate();

    factory.CleanTreeBlocs( "stars generator" );

    */
    
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