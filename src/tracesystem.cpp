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

#include "tracesystem.h"
#include "bodyaspect.h"
#include "cameraaspect.h"
#include "physicsaspect.h"
#include "proceduralaspect.h"
#include "renderingaspect.h"
#include "timeaspect.h"
#include "transformaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Systems;

_DECLARE_DS_LOGGER( logger, "tracesystem", NULL )

class ClassDump
{
public:
    static dsstring m_text;

    template<typename BaseClass, typename T>
    static void Dump( BaseClass* p_bloc )
    {
        if( dynamic_cast<T*>( p_bloc ) )
        {
            m_text = typeid(T).name();
        }     
    }
};

dsstring ClassDump::m_text;


void TraceSystem::SetLogConf( DrawSpace::Logger::Configuration* p_logconf )
{
    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );
}

bool TraceSystem::Init( EntityGraph::EntityNodeGraph* p_entitygraph )
{
    return true;
}

void TraceSystem::Release( EntityGraph::EntityNodeGraph* p_entitygraph )
{
}

void TraceSystem::Run( EntityGraph::EntityNodeGraph* p_entitygraph )
{
    if( p_entitygraph->ToDump() )
    {
    
        _DSDEBUG( logger, " *********************** Entity dump begin ***********************" )

        p_entitygraph->AcceptSystemRootToLeaf( this );

        _DSDEBUG( logger, " *********************** Entity dump end *************************" )

        p_entitygraph->ResetDumpFlag();
    }
}





void TraceSystem::VisitEntity( Core::Entity* p_parent, Core::Entity* p_entity )
{
    _DSDEBUG( logger, " " )
    _DSDEBUG( logger, dsstring ( " ENTITY : " ) << p_entity )

    if( NULL == p_parent )
    {
        _DSDEBUG( logger, "is root" )
    }
    else
    {
        _DSDEBUG( logger, dsstring ( " -------------------->>>>> child of : " ) << p_parent )
    }

    std::vector<Core::Aspect*> aspects;
    p_entity->GetAllAspects( aspects );

    _DSDEBUG( logger, dsstring ( " has " ) << aspects.size() << dsstring( " aspect(s)" ) )

    for( size_t i = 0; i < aspects.size(); i++ )
    {
        // find aspect's name to dump it on trace...
        ClassDump::Dump<DrawSpace::Core::Aspect, Aspect::BodyAspect>( aspects[i] );
        ClassDump::Dump<DrawSpace::Core::Aspect, Aspect::CameraAspect>( aspects[i] );
        ClassDump::Dump<DrawSpace::Core::Aspect, Aspect::PhysicsAspect>( aspects[i] );
        ClassDump::Dump<DrawSpace::Core::Aspect, Aspect::ProceduralAspect>( aspects[i] );
        ClassDump::Dump<DrawSpace::Core::Aspect, Aspect::RenderingAspect>( aspects[i] );
        ClassDump::Dump<DrawSpace::Core::Aspect, Aspect::TimeAspect>( aspects[i] );
        ClassDump::Dump<DrawSpace::Core::Aspect, Aspect::TransformAspect>( aspects[i] );

        _DSDEBUG( logger, dsstring ( " aspect " ) << i << dsstring( " is " ) << ClassDump::m_text )

        Aspect::ProceduralAspect* procedural_aspect = dynamic_cast<Aspect::ProceduralAspect*>( aspects[i] );

        /////////////////////////////////dump detaille aspect procedural......
        if( procedural_aspect )
        {
            ClassDump::m_text = "unknown";

            ComponentList<Aspect::ProceduralAspect::ProceduralBloc*> components;
            procedural_aspect->GetComponentsByType<Aspect::ProceduralAspect::ProceduralBloc*>( components );

            _DSDEBUG( logger, dsstring ( "procedural aspect has " ) << components.size() << dsstring( " procedural component(s)" ) )

            for( size_t j = 0; j < components.size(); j++ )
            {
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::RootProceduralBloc>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::SeedSourceProceduralBloc>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::PublishProceduralBloc>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::SimpleValueProceduralBloc<int>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::SimpleValueProceduralBloc<long>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::SimpleValueProceduralBloc<float>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::SimpleValueProceduralBloc<dsreal>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::SimpleValueProceduralBloc<dsstring>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::UniformRandomIntValueProceduralBloc<int>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::UniformRandomIntValueProceduralBloc<long>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::UniformRandomIntValueProceduralBloc<size_t>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::ArrayProceduralBloc<int>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::ArrayProceduralBloc<long>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::ArrayProceduralBloc<float>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::ArrayProceduralBloc<dsreal>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::ArrayProceduralBloc<dsstring>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::RepeatProceduralBloc<int>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::RepeatProceduralBloc<long>>( components[j]->getPurpose() );
                ClassDump::Dump<DrawSpace::Aspect::ProceduralAspect::ProceduralBloc, Aspect::ProceduralAspect::RepeatProceduralBloc<size_t>>( components[j]->getPurpose() );


                _DSDEBUG( logger, dsstring ( "procedural aspect component " ) << i << dsstring( " is " ) << ClassDump::m_text )

                Aspect::ProceduralAspect::ValueProceduralBloc<int>* pint = dynamic_cast<Aspect::ProceduralAspect::ValueProceduralBloc<int>*>( components[j]->getPurpose() );
                if( pint )
                {
                    _DSDEBUG( logger, dsstring ( "value = " ) << pint->GetValue() );
                }
                else
                {
                    Aspect::ProceduralAspect::ValueProceduralBloc<long>* plong = dynamic_cast<Aspect::ProceduralAspect::ValueProceduralBloc<long>*>( components[j]->getPurpose() );
                    if( plong )
                    {
                        _DSDEBUG( logger, dsstring ( "value = " ) << plong->GetValue() );
                    }
                    else
                    {
                        Aspect::ProceduralAspect::ValueProceduralBloc<float>* pfloat = dynamic_cast<Aspect::ProceduralAspect::ValueProceduralBloc<float>*>( components[j]->getPurpose() );
                        if( pfloat )
                        {
                            _DSDEBUG( logger, dsstring ( "value = " ) << pfloat->GetValue() );
                        }
                        else
                        {
                            Aspect::ProceduralAspect::ValueProceduralBloc<dsreal>* preal = dynamic_cast<Aspect::ProceduralAspect::ValueProceduralBloc<dsreal>*>( components[j]->getPurpose() );
                            if( preal )
                            {
                                _DSDEBUG( logger, dsstring ( "value = " ) << preal->GetValue() );
                            }
                            else
                            {
                                Aspect::ProceduralAspect::RootProceduralBloc* proot = dynamic_cast<Aspect::ProceduralAspect::RootProceduralBloc*>( components[j]->getPurpose() );

                                if( proot )
                                {
                                    // chercher le component de type string qui doit normalement accompagner ce component procedural root

                                    ComponentList<dsstring> components_string;
                                    procedural_aspect->GetComponentsByType<dsstring>( components_string );

                                    if( components_string.size() > 0 )
                                    {
                                        _DSDEBUG( logger, dsstring ( "string component = " ) << components_string[0]->getPurpose() );
                                    }
                                    else
                                    {
                                        _DSDEBUG( logger, dsstring ( "WARNING ! no string component found with procedural root !!" ) );    
                                    }
                                }                     
                            }
                        }                    
                    }                
                }
            }
        }
        //////////////////////////////////////////////////////

    }

    _DSDEBUG( logger, " " )
}