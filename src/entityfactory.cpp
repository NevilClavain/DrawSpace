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


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::EntityGraph;

bool Factory::BuildFromFile( const std::string& p_filepath, DrawSpace::EntityGraph::EntityNode& p_node )
{
    m_parser_state = EXPECT_ENTITY_DECL;
    JSONParser parser;
    int token_index = 1;

    parser.ParseFromFile( p_filepath );

    if( JSMN_OBJECT == parser.GetTokenType( 0 ) )
    {
        recurs_explore_entities( parser, token_index, NULL, NULL );
    }
    else
    {
        _DSEXCEPTION( "JSON parse : unexpected type for token 0" );
    }

    return true;
}

void Factory::recurs_explore_entities( JSONParser& p_parser, int& p_token_index, DrawSpace::Core::Entity* p_entity, DrawSpace::EntityGraph::EntityNode* p_entityNode )
{
    DrawSpace::Core::Entity*             entity = NULL;
    DrawSpace::EntityGraph::EntityNode*  entityNode = NULL;

    int type0 = p_parser.GetTokenType( p_token_index );
    int size0 = p_parser.GetTokenSize( p_token_index );
    int type1 = p_parser.GetTokenType( p_token_index + 1 );
    int size1 = p_parser.GetTokenSize( p_token_index + 1 );

    switch( m_parser_state )
    {
        case EXPECT_ENTITY_DECL:
        {
            dsstring name;
            if( JSMN_STRING == type0 )
            {
                p_parser.GetTokenString( p_token_index, name );

                if( "Entity" == name )
                {
                    if( JSMN_OBJECT == type1 )
                    {
                        // ici creer une nvelle entite
                        //////

                        p_token_index += 2;

                        m_parser_state = EXPECT_ENTITY_ARGS;
                        for( int i = 0; i < size1; i++ )
                        {
                            recurs_explore_entities( p_parser, p_token_index, entity, entityNode );
                        }
                    }
                    else
                    {
                        _DSEXCEPTION( "JSON parse : unexpected type for 2nd token (object expected)" );
                    }                
                }
                else
                {
                    _DSEXCEPTION( "JSON parse : unexpected value for string in 1st token (Entity expected)" );
                }
            }
            else
            {
                _DSEXCEPTION( "JSON parse : unexpected type for 1st token (string expected)" );
            }            
        }
        break;

        case EXPECT_ENTITY_ARGS:
        {
            dsstring name;
            if( JSMN_STRING == type0 )
            {
                p_parser.GetTokenString( p_token_index, name );

                if( "Aspects" == name )
                {
                    if( JSMN_ARRAY == type1 )
                    {
                        p_token_index += 1 + 1 + size1; // le token string "Array" + le token array lui meme + la taille de son contenu
                    }
                    else
                    {
                        _DSEXCEPTION( "JSON parse : unexpected type for 2nd token ( Aspects -> array expected)" );
                    } 
                }
                else if( "Children" == name )
                {
                    if( JSMN_ARRAY == type1 )
                    {
                        p_token_index++; // pointe sur le array

                        for( int i = 0; i < size1; i++ )
                        {
                            int type2 = p_parser.GetTokenType( p_token_index + 1 );
                            int size2 = p_parser.GetTokenSize( p_token_index + 1 );

                            if( JSMN_OBJECT == type2 )
                            {
                                if( size2 > 0 )
                                {
                                    p_token_index += 2;
                                    m_parser_state = EXPECT_ENTITY_DECL;
                                    recurs_explore_entities( p_parser, p_token_index, NULL, NULL );
                                }
                            }
                            else
                            {
                                _DSEXCEPTION( "JSON parse : unexpected type for token in array ( object expected)" );
                            }
                        }

                        p_token_index++; // pointe sur apres le array
                    }
                    else
                    {
                        _DSEXCEPTION( "JSON parse : unexpected type for 2nd token ( Children -> array expected)" );
                    } 
                }
                else if( "Id" == name )
                {
                    if( JSMN_STRING == type1 )
                    {
                        p_parser.GetTokenString( p_token_index + 1, name );


                        p_token_index += 2;
                    }
                    else
                    {
                        _DSEXCEPTION( "JSON parse : unexpected type for entity Id ( string expected)" );
                    }
                }
                else
                {
                    _DSEXCEPTION( "JSON parse : unexpected value for string in 1st token (Aspects or Children expected)" );
                }
            }
            else
            {
                _DSEXCEPTION( "JSON parse : unexpected type for 1st token (string expected)" );
            }            
        }
        break;
    }

}