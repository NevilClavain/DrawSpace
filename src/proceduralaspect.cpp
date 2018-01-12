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
    if( m_to_update )
    {
        update();
        m_to_update = false;
    }
}

void ProceduralAspect::SetToUpdate( void )
{
    m_to_update = true;
}

void ProceduralAspect::update( void )
{
    ComponentList<ProceduralAspect::Operation> operations;
    GetComponentsByType<ProceduralAspect::Operation>( operations );

    switch( operations[0]->getPurpose() )
    {
        case PUBLISH:

            _asm nop
            break;    
    }
}