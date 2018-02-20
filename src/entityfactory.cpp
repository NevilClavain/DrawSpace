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

Factory::Factory( DrawSpace::Systems::Hub& p_hub ) :
m_object_content_cb( this, &Factory::on_object_content ),
m_array_content_cb( this, &Factory::on_array_content ),
m_array_object_content_cb( this, &Factory::on_array_object_content ),
m_string_content_cb( this, &Factory::on_string_content ),
m_num_content_cb( this, &Factory::on_num_content ),
m_hub( p_hub )
{
}

bool Factory::BuildFromFile( const std::string& p_filepath, DrawSpace::EntityGraph::EntityNode& p_node, ProceduralAspect::PublishProceduralBloc::ProceduralPublicationEventHandler* p_pub_evt_handler )
{
    JSONParser parser;
    parser.ParseFromFile( p_filepath );
    
    m_parser_data.clear();

    ParserDataImpl parser_data;
    parser_data.m_data.m_parser_state = EXPECT_ENTITY_DECL;
    parser_data.m_data.m_entity_data.first = p_node;
    parser_data.m_data.m_entity_data.second = NULL;

    m_pub_evt_handlers = p_pub_evt_handler;

    parser.AnalyzeTokens( &parser_data, &m_object_content_cb, &m_array_content_cb, &m_array_object_content_cb, &m_string_content_cb, &m_num_content_cb );
    
    return true;
}

JSONParser::UserData* Factory::on_object_content( JSONParser::UserData* p_userdata, const dsstring& p_owner_id, const dsstring& p_id, JSONParser::ParseState p_parser_state )
{
    ParserDataImpl* parent_parser_data = static_cast<ParserDataImpl*>( p_userdata );
    ParserState parser_state = parent_parser_data->m_data.m_parser_state;

    switch( parser_state )
    {
        case EXPECT_ENTITY_DECL:
        {
            if( "Entity" == p_id )
            {
                if( JSONParser::JSON_NODE_PARSE_BEGIN == p_parser_state )
                {
                    DrawSpace::Core::Entity* entity = _DRAWSPACE_NEW_( DrawSpace::Core::Entity, DrawSpace::Core::Entity );
                    DrawSpace::EntityGraph::EntityNode node = parent_parser_data->m_data.m_entity_data.first.AddChild( entity );

                    ParserDataImpl parser_data;
                    parser_data.m_data.m_parser_state = EXPECT_ENTITY_ARGS;
                    parser_data.m_data.m_entity_data.first = node;
                    parser_data.m_data.m_entity_data.second = entity;

                    m_parser_data.push_back( parser_data );

                    JSONParser::UserData* userdata = &m_parser_data.back();
                    return userdata;
                }
            }                
        }
        break;

        case EXPECT_ASPECT_ARGS:
        {
            if( "ProceduralAspect" == p_id )
            {
                if( JSONParser::JSON_NODE_PARSE_BEGIN == p_parser_state )
                {
                    DrawSpace::Core::Entity* entity = parent_parser_data->m_data.m_entity_data.second;
                    entity->AddAspect<ProceduralAspect>();

                    ParserDataImpl parser_data;
                    parser_data.m_data.m_parser_state = EXPECT_PROCEDURAL_ASPECT_COMPONENT_DECL;
                    parser_data.m_data.m_entity_data.first = parent_parser_data->m_data.m_entity_data.first;
                    parser_data.m_data.m_entity_data.second = parent_parser_data->m_data.m_entity_data.second;

                    m_parser_data.push_back( parser_data );

                    JSONParser::UserData* userdata = &m_parser_data.back();
                    return userdata;
                }
            }   
        }
        break;
        
        case EXPECT_PROCEDURAL_ASPECT_COMPONENT_ARGS:
        {
            if( "RootProceduralBloc" == p_id )
            {
                if( JSONParser::JSON_NODE_PARSE_END == p_parser_state )
                {
                    if( m_procedural_bloc_strings_args.count( "Group" ) && m_procedural_bloc_strings_args.count( "BlocId" ) )
                    {
                        dsstring group = m_procedural_bloc_strings_args["Group"];
                        dsstring BlocId = m_procedural_bloc_strings_args["BlocId"];
                   
                        ProceduralAspect* procedural_aspect = parent_parser_data->m_data.m_entity_data.second->GetAspect<ProceduralAspect>();
                        procedural_aspect->AddComponent<dsstring>( "name", group );
                        procedural_aspect->AddComponent<ProceduralAspect::ProceduralBloc*>( BlocId, m_hub.GetProceduralFactory().CreateBloc<ProceduralAspect::RootProceduralBloc>( group ) );


                        m_procedural_bloc_strings_args.clear();
                    }
                }
            }
            else if( "UniformRandomIntValueProceduralBloc" == p_id )
            {
                if( JSONParser::JSON_NODE_PARSE_END == p_parser_state )
                {
                    if( m_procedural_bloc_strings_args.count( "Group" ) && m_procedural_bloc_strings_args.count( "BlocId" ) )
                    {
                        dsstring group = m_procedural_bloc_strings_args["Group"];
                        dsstring BlocId = m_procedural_bloc_strings_args["BlocId"];
                   
                        ProceduralAspect* procedural_aspect = parent_parser_data->m_data.m_entity_data.second->GetAspect<ProceduralAspect>();
                        procedural_aspect->AddComponent<ProceduralAspect::ProceduralBloc*>( BlocId, m_hub.GetProceduralFactory().CreateBloc<ProceduralAspect::UniformRandomIntValueProceduralBloc<long>>( group ) );


                        m_procedural_bloc_strings_args.clear();
                    }
                }
            }
            else if( "RepProceduralBloc" == p_id )
            {
                if( JSONParser::JSON_NODE_PARSE_END == p_parser_state )
                {
                    if( m_procedural_bloc_strings_args.count( "Group" ) && m_procedural_bloc_strings_args.count( "BlocId" ) )
                    {
                        dsstring group = m_procedural_bloc_strings_args["Group"];
                        dsstring BlocId = m_procedural_bloc_strings_args["BlocId"];
                   
                        ProceduralAspect* procedural_aspect = parent_parser_data->m_data.m_entity_data.second->GetAspect<ProceduralAspect>();

                        procedural_aspect->AddComponent<ProceduralAspect::ProceduralBloc*>( BlocId, m_hub.GetProceduralFactory().CreateBloc<ProceduralAspect::RepeatProceduralBloc<int>>( group ) );

                        m_procedural_bloc_strings_args.clear();
                    }
                }
            }
            else if( "IntegerProceduralBloc" == p_id )
            {
                if( JSONParser::JSON_NODE_PARSE_END == p_parser_state )
                {
                    if( m_procedural_bloc_strings_args.count( "Group" ) && m_procedural_bloc_strings_args.count( "BlocId" ) &&  
                        m_procedural_bloc_num_args.count( "Value" ) )
                    {
                        dsstring group = m_procedural_bloc_strings_args["Group"];
                        dsstring BlocId = m_procedural_bloc_strings_args["BlocId"];
                        int nb_loop = m_procedural_bloc_num_args["Value"];
                   
                        ProceduralAspect* procedural_aspect = parent_parser_data->m_data.m_entity_data.second->GetAspect<ProceduralAspect>();

                        procedural_aspect->AddComponent<ProceduralAspect::ProceduralBloc*>( BlocId, m_hub.GetProceduralFactory().CreateBloc<ProceduralAspect::SimpleValueProceduralBloc<long>>( group, nb_loop ) );

                        m_procedural_bloc_strings_args.clear();
                    }
                }            
            }
            else if( "PublishProceduralBloc" == p_id )
            {
                if( JSONParser::JSON_NODE_PARSE_END == p_parser_state )
                {
                    if( m_procedural_bloc_strings_args.count( "Group" ) && m_procedural_bloc_strings_args.count( "BlocId" ) &&  
                        m_procedural_bloc_strings_args.count( "MessageId" ) )
                    {
                        dsstring group = m_procedural_bloc_strings_args["Group"];
                        dsstring BlocId = m_procedural_bloc_strings_args["BlocId"];
                        dsstring MessageId = m_procedural_bloc_strings_args["MessageId"];

                        ProceduralAspect* procedural_aspect = parent_parser_data->m_data.m_entity_data.second->GetAspect<ProceduralAspect>();
                        ProceduralAspect::PublishProceduralBloc* pub = m_hub.GetProceduralFactory().CreateBloc<ProceduralAspect::PublishProceduralBloc>( group, MessageId );
                        pub->m_proc_pub_evt_handlers.insert( m_pub_evt_handlers );
                        procedural_aspect->AddComponent<ProceduralAspect::ProceduralBloc*>( BlocId, pub );
                    }
                }                
            }            
        }
        break;
        
    }

    return p_userdata;
}

JSONParser::UserData* Factory::on_array_content( JSONParser::UserData* p_userdata, const dsstring& p_owner_id, const dsstring& p_id, JSONParser::ParseState p_parser_state )
{
    if( JSONParser::JSON_NODE_PARSE_BEGIN == p_parser_state )
    {
        ParserDataImpl* parent_parser_data = static_cast<ParserDataImpl*>( p_userdata );
        ParserState parser_state = parent_parser_data->m_data.m_parser_state;

        switch( parser_state )
        {
            case EXPECT_ENTITY_ARGS:
            {
                if( "Aspects" == p_id )
                {
                    ParserDataImpl parser_data;
                    parser_data.m_data.m_parser_state = EXPECT_ASPECT_ARGS;
                    parser_data.m_data.m_entity_data.first = parent_parser_data->m_data.m_entity_data.first;
                    parser_data.m_data.m_entity_data.second = parent_parser_data->m_data.m_entity_data.second;

                    m_parser_data.push_back( parser_data );

                    JSONParser::UserData* userdata = &m_parser_data.back();
                    return userdata;           
                }
                else if( "Children" == p_id )
                {
                    ParserDataImpl parser_data;
                    parser_data.m_data.m_parser_state = EXPECT_ENTITY_DECL;
                    parser_data.m_data.m_entity_data.first = parent_parser_data->m_data.m_entity_data.first;
                    parser_data.m_data.m_entity_data.second = parent_parser_data->m_data.m_entity_data.second;

                    m_parser_data.push_back( parser_data );

                    JSONParser::UserData* userdata = &m_parser_data.back();
                    return userdata;
                }
            }
            break;

            case EXPECT_PROCEDURAL_ASPECT_COMPONENT_DECL:
            {
                if( "Components" == p_id )
                {
                    ParserDataImpl parser_data;
                    parser_data.m_data.m_parser_state = EXPECT_PROCEDURAL_ASPECT_COMPONENT_ARGS;
                    parser_data.m_data.m_entity_data.first = parent_parser_data->m_data.m_entity_data.first;
                    parser_data.m_data.m_entity_data.second = parent_parser_data->m_data.m_entity_data.second;

                    m_parser_data.push_back( parser_data );

                    JSONParser::UserData* userdata = &m_parser_data.back();
                    return userdata;            
                }        
            }
        }
    }
    return p_userdata;
}

JSONParser::UserData* Factory::on_array_object_content( JSONParser::UserData* p_userdata, const dsstring& p_owner_id, int p_index, JSONParser::ParseState p_parser_state )
{
    return p_userdata;
}

JSONParser::UserData* Factory::on_string_content( JSONParser::UserData* p_userdata, const dsstring& p_owner_id, const dsstring& p_id, const dsstring& p_str )
{
    ParserDataImpl* parent_parser_data = static_cast<ParserDataImpl*>( p_userdata );
    ParserState parser_state = parent_parser_data->m_data.m_parser_state;

    switch( parser_state )
    {
        case EXPECT_ENTITY_ARGS:
        {
            if( "Id" == p_id )
            {
                m_nodes[p_str] = parent_parser_data->m_data.m_entity_data;
            }
        }
        break;

        case EXPECT_PROCEDURAL_ASPECT_COMPONENT_ARGS:
        {
            m_procedural_bloc_strings_args[p_id] = p_str;        
        }

        /*
        case EXPECT_PROCEDURAL_ASPECT_COMPONENT_ARGS:
        {
            if( "RootProceduralBloc" == p_id )
            {
                ProceduralAspect* procedural_aspect = parent_parser_data->m_data.m_entity_data.second->GetAspect<ProceduralAspect>();
                procedural_aspect->AddComponent<dsstring>( "name", p_str );
                procedural_aspect->AddComponent<ProceduralAspect::ProceduralBloc*>( "root", m_hub.GetProceduralFactory().CreateBloc<ProceduralAspect::RootProceduralBloc>( p_str ) );
            }
        }
        break;
        
        case EXPECT_PROCEDURAL_PUBLISHER_ARGS:
        {
            ProceduralAspect* procedural_aspect = parent_parser_data->m_data.m_entity_data.second->GetAspect<ProceduralAspect>();
            ProceduralAspect::PublishProceduralBloc* pub = m_hub.GetProceduralFactory().CreateBloc<ProceduralAspect::PublishProceduralBloc>( p_str, p_id );
            pub->m_proc_pub_evt_handlers.insert( m_pub_evt_handlers );
            procedural_aspect->AddComponent<ProceduralAspect::ProceduralBloc*>( p_id, pub );
        }
        break;
        */
    }
    return p_userdata;
}

JSONParser::UserData* Factory::on_num_content( JSONParser::UserData* p_userdata, const dsstring& p_owner_id, const dsstring& p_id, dsreal p_val )
{
    ParserDataImpl* parent_parser_data = static_cast<ParserDataImpl*>( p_userdata );
    ParserState parser_state = parent_parser_data->m_data.m_parser_state;

    switch( parser_state )
    {
        case EXPECT_PROCEDURAL_ASPECT_COMPONENT_ARGS:
        {
            m_procedural_bloc_num_args[p_id] = p_val;        
        }

    }
    return p_userdata;
}
