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

#include "spacebox.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;

Spacebox::Spacebox( void )
{

}

Spacebox::~Spacebox( void )
{

}

void Spacebox::GetDescr( dsstring& p_descr )
{

}

void Spacebox::DumpMemoryAllocs( void )
{

}

void Spacebox::SetRenderer( Renderer * p_renderer )
{

}

void Spacebox::OnRegister( Scenegraph* p_scenegraph )
{

}

bool Spacebox::LoadAssets( void )
{
    return false;
}

Core::Meshe* Spacebox::GetMeshe( const dsstring& p_mesheid )
{
    return NULL;
}

void Spacebox::RegisterPassSlot( const dsstring p_passname )
{

}

void Spacebox::GetNodeFromPass( const dsstring p_passname, const dsstring& p_nodeid )
{

}

