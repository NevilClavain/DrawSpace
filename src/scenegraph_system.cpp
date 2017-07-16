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

#include "scenegraph_system.h"

#include "memalloc.h"
#include "ac3dmeshe.h"
#include "components_ids.h"
#include "meshe_component.h"

#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;


SceneGraphSystem::SceneGraphSystem( void )
{
    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();
}

SceneGraphSystem::~SceneGraphSystem( void )
{
    delete m_meshe_import;
}

void SceneGraphSystem::VisitEntitySet( Entity* p_entity, EntitySet::Phase p_phase )
{
    switch( p_phase )
    {
        case EntitySet::PHASE_INIT:
            
            phase_init( p_entity );
            break;

        case EntitySet::PHASE_RELEASE:

            phase_release( p_entity );
            break;

        case EntitySet::PHASE_RUN:

            phase_run( p_entity );
            break;
    }
}

void SceneGraphSystem::phase_init( Entity* p_entity )
{   
    if( p_entity->CheckComponent( MesheComponentType ) )
    {
        MesheComponent* meshe_comp = p_entity->ExtractComponent<MesheComponent>( MesheComponentType, 0 );

        meshe_comp->m_meshe = _DRAWSPACE_NEW_( Meshe, Meshe );
        meshe_comp->m_meshe->SetImporter( m_meshe_import );

        if( meshe_comp->m_filepath != "" )
        {
             if( false == meshe_comp->m_meshe->LoadFromFile( meshe_comp->m_filepath, meshe_comp->m_index_in_file ) )
             {                
                _DSEXCEPTION( "Scenegraph system init phase : cannot load meshe : " + meshe_comp->m_filepath );               
             }
        }     
    }
}

void SceneGraphSystem::phase_release( Entity* p_entity )
{
    if( p_entity->CheckComponent( MesheComponentType ) )
    {
        MesheComponent* meshe_comp = p_entity->ExtractComponent<MesheComponent>( MesheComponentType, 0 );

        _DRAWSPACE_DELETE_( meshe_comp->m_meshe );
    }
}

void SceneGraphSystem::phase_run( Entity* p_entity )
{
 
}
