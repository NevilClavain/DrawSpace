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

#include "proceduralaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;


ProceduralAspect::ProceduralAspect( void ) :
m_to_update( false )
{

}

void ProceduralAspect::Run( Core::Entity* p_parent, Core::Entity* p_entity )
{   
    update();
}

void ProceduralAspect::SetToUpdate( bool p_state )
{
    m_to_update = p_state;
}

bool ProceduralAspect::GetToUpdate( void ) const
{
    return m_to_update;
}

void ProceduralAspect::update( void )
{   
    ComponentList<size_t> operations;
    GetComponentsByType<size_t>( operations );

    /*
    if( PROCEDURALBLOCID(RootProceduralBloc) == operations[0]->getPurpose() )
    {
        _asm nop
    }
    else if( PROCEDURALBLOCID(PublishProceduralBloc) == operations[0]->getPurpose() )
    {
        _asm nop
    } 
    */
}