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

    template<typename T>
    static void Dump( DrawSpace::Core::Aspect* p_aspect )
    {
        if( dynamic_cast<T*>( p_aspect ) )
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
        // find aspect's name to sump it on trace...
        ClassDump::Dump<Aspect::BodyAspect>( aspects[i] );
        ClassDump::Dump<Aspect::CameraAspect>( aspects[i] );
        ClassDump::Dump<Aspect::PhysicsAspect>( aspects[i] );
        ClassDump::Dump<Aspect::ProceduralAspect>( aspects[i] );
        ClassDump::Dump<Aspect::RenderingAspect>( aspects[i] );
        ClassDump::Dump<Aspect::TimeAspect>( aspects[i] );
        ClassDump::Dump<Aspect::TransformAspect>( aspects[i] );

        _DSDEBUG( logger, dsstring ( " aspect " ) << i << dsstring( " is " ) << ClassDump::m_text )
    

    }

    _DSDEBUG( logger, " " )
}