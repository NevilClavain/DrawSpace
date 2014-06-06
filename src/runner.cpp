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

#include "runner.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


Runner::Runner( void )
{

}

Runner::~Runner( void )
{
    std::vector<std::pair<DrawSpace::Utils::Mutex*,IProperty*>>::iterator it;
}

void Runner::Run( void )
{

}

void Runner::RegisterEventHandler( const dsstring& p_event_name, MediatorEventHandler* p_handler )
{
    m_handlers[p_event_name] = p_handler;
}

PropertyPool* Runner::GetPropertyPool( void )
{
    return &m_propertypool;
}