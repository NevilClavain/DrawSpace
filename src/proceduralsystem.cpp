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
}

void ProceduralSystem::VisitEntity( Core::Entity* p_parent, Core::Entity* p_entity )
{
    ProceduralAspect* procedural_aspect = p_entity->GetAspect<ProceduralAspect>();
    if( procedural_aspect )
    {
        ComponentList<ProceduralAspect::Operation> operations;
        procedural_aspect->GetComponentsByType<ProceduralAspect::Operation>( operations );

        if( ProceduralAspect::ROOT == operations[0]->getPurpose() )
        {
            // si true, on va executer une fois cette hierarchie d'entites avec aspect procedural (sous ce ROOT) 
            // jusqu'a rencontrer un prochain root procedural...

            // pour cette raison, eviter d'imbriquer les ROOT dans l'entity Tree ! (pas mettre de ROOT procedural sous un autre ROOT procedural!)
            // chacun chez soi, et les vaches seront bien gardees ;)

            m_exec_flag = procedural_aspect->GetToUpdate();

        }

        if( m_exec_flag )
        {
            procedural_aspect->Run( p_parent, p_entity );
            procedural_aspect->SetToUpdate( false ); // one shot exec for this root and all its children
        }
    }
}