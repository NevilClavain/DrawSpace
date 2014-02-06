/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "world.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;

World::World( void )
{
    m_collisionConfiguration    = _DRAWSPACE_NEW_( btDefaultCollisionConfiguration, btDefaultCollisionConfiguration );
    m_collisionDispatcher       = _DRAWSPACE_NEW_( btCollisionDispatcher, btCollisionDispatcher( m_collisionConfiguration ) );
    m_broadphase                = _DRAWSPACE_NEW_( btDbvtBroadphase, btDbvtBroadphase );
}

World::~World( void )
{
    _DRAWSPACE_DELETE_( m_collisionConfiguration );
    _DRAWSPACE_DELETE_( m_collisionDispatcher );
    _DRAWSPACE_DELETE_( m_broadphase );
}

bool World::Initialize( void )
{
    return true;
}
