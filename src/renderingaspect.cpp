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


#include "renderingaspect.h"
#include "entity.h"
#include "entitynodegraph.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Interface;

RenderingAspect::RenderingAspect( void ) :
m_entitynodegraph( NULL )
{
}

void RenderingAspect::AddImplementation( AspectImplementations::RenderingAspectImpl* p_impl )
{
    m_impls.push_back( p_impl );
    p_impl->SetOwner( this );
    if( m_entitynodegraph )
    {
        p_impl->SetEntityNodeGraph( m_entitynodegraph );
    }

    p_impl->Init( m_owner );
}

void RenderingAspect::RemoveImplementation( DrawSpace::Interface::AspectImplementations::RenderingAspectImpl* p_impl )
{
    for( auto it = m_impls.begin(); it != m_impls.end(); ++it )
    {
        if( *it == p_impl )
        {
            p_impl->Release();

            if (m_entitynodegraph)
            {
                p_impl->SetEntityNodeGraph(NULL);
            }
            m_impls.erase( it );
            break;
        }
    } 
}

void RenderingAspect::Run( Entity* p_owner_entity, bool p_drawtextlements )
{    
    for( size_t i = 0; i < m_impls.size(); i++ )
    {
        if( p_drawtextlements == m_impls[i]->IsText() )
        {
            m_impls[i]->Run( p_owner_entity );
        }
    }
}

void RenderingAspect::OnAddedInGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph)
{
    m_entitynodegraph = p_entitynodegraph;
    for(auto& e : m_impls)
    {
        e->SetEntityNodeGraph(p_entitynodegraph);
    }
}

void RenderingAspect::OnRemovedFromGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph)
{
    m_entitynodegraph = p_entitynodegraph;
    for(auto& e : m_impls)
    {
        e->SetEntityNodeGraph(NULL);
    }
}
