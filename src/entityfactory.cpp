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

#include "entityfactory.h"
#include "proceduralaspect.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Aspect;

Factory::Factory( void ) :
m_object_content_cb( this, &Factory::on_object_content ),
m_array_content_cb( this, &Factory::on_array_content ),
m_string_content_cb( this, &Factory::on_string_content )
{
}

bool Factory::BuildFromFile( const std::string& p_filepath, DrawSpace::EntityGraph::EntityNode& p_node )
{
    m_parser_state = EXPECT_ENTITY_DECL;
    JSONParser parser;
    int token_index = 1;

    parser.ParseFromFile( p_filepath );
    parser.AnalyzeTokens( &m_object_content_cb, &m_array_content_cb, &m_string_content_cb );
    
    return true;
}

void Factory::on_object_content( const dsstring& p_owner_id, const dsstring& p_id )
{
}

void Factory::on_array_content( const dsstring& p_owner_id, const dsstring& p_id )
{
}

void Factory::on_string_content( const dsstring& p_owner_id, const dsstring& p_id, const dsstring& p_str )
{

}
