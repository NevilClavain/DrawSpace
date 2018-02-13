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
m_array_object_content_cb( this, &Factory::on_array_object_content ),
m_string_content_cb( this, &Factory::on_string_content ),
m_num_content_cb( this, &Factory::on_num_content )
{
}

bool Factory::BuildFromFile( const std::string& p_filepath, DrawSpace::EntityGraph::EntityNode& p_node )
{    
    JSONParser parser;

    parser.ParseFromFile( p_filepath );
    
    m_parser_data.clear();
    
    ParserData parser_data;
    EntityData entity_data;
    entity_data.first = p_node;
    entity_data.second = NULL;
    parser_data.m_data = entity_data;
    
    parser.AnalyzeTokens( &parser_data, &m_object_content_cb, &m_array_content_cb, &m_array_object_content_cb, &m_string_content_cb, &m_num_content_cb );
    
    return true;
}

JSONParser::UserData* Factory::on_object_content( JSONParser::UserData* p_userdata, const dsstring& p_owner_id, const dsstring& p_id )
{
    if( "Entity" == p_id )
    {
        ParserData* parent_parser_data = static_cast<ParserData*>( p_userdata );
        
        DrawSpace::Core::Entity* entity = _DRAWSPACE_NEW_( DrawSpace::Core::Entity, DrawSpace::Core::Entity );
        DrawSpace::EntityGraph::EntityNode node = parent_parser_data->m_data.first.AddChild( entity );

        ParserData parser_data;
        EntityData entity_data;
        entity_data.first = node;
        entity_data.second = entity;
        parser_data.m_data = entity_data;

        m_parser_data.push_back( parser_data );

        JSONParser::UserData* userdata = &m_parser_data.back();
        return userdata;
    }
    else if( "ProceduralAspect" == p_id )
    {
        ParserData* parent_parser_data = static_cast<ParserData*>( p_userdata );
        DrawSpace::Core::Entity* entity = parent_parser_data->m_data.second;

        entity->AddAspect<ProceduralAspect>();    
    }

    return p_userdata;
}

JSONParser::UserData* Factory::on_array_content( JSONParser::UserData* p_userdata, const dsstring& p_owner_id, const dsstring& p_id )
{
    return p_userdata;
}

JSONParser::UserData* Factory::on_array_object_content( JSONParser::UserData* p_userdata, const dsstring& p_owner_id, int p_index )
{
    return p_userdata;
}

JSONParser::UserData* Factory::on_string_content( JSONParser::UserData* p_userdata, const dsstring& p_owner_id, const dsstring& p_id, const dsstring& p_str )
{
    if( "Id" == p_id )
    {
        ParserData* parent_parser_data = static_cast<ParserData*>( p_userdata );

        m_nodes[p_str] = parent_parser_data->m_data;
    }
    return p_userdata;
}

JSONParser::UserData* Factory::on_num_content( JSONParser::UserData* p_userdata, const dsstring& p_owner_id, const dsstring& p_id, dsreal p_val )
{
    return p_userdata;
}
