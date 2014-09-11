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

#include "factory.h"
#include "asset.h"

DrawSpace::Core::Factory* DrawSpace::Core::Factory::m_instance = NULL;

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Factory::Factory( void )
{


}

Factory::~Factory( void )
{

}

bool Factory::ExecuteFromTextFile( const dsstring& p_path )
{
    return false;
}

bool Factory::ExecuteFromBinaryFile( const dsstring& p_path )
{
    return false;
}

bool Factory::ExecuteFromArchiveChunk( const DrawSpace::Utils::Archive& p_arc )
{
    return false;
}

bool Factory::ExecuteFromTextChunk( const dsstring& p_text )
{
    return false;
}
