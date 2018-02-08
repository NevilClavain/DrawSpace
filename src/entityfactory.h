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

#ifndef _ENTITYFACTORY_H_
#define _ENTITYFACTORY_H_

#include "entitynodegraph.h"
#include "jsonparser.h"

namespace DrawSpace
{
namespace EntityGraph
{
class Factory
{
protected:

    using EntityData = std::pair<DrawSpace::EntityGraph::EntityNode, DrawSpace::Core::Entity*>;

    typedef enum
    {
        EXPECT_ENTITY_DECL,
        EXPECT_ENTITY_ARGS,
        EXPECT_ENTITY_ASPECTS_ARGS,
        EXPECT_ASPECT_ARGS,
    
    } ParserState;

    ParserState                     m_parser_state;  
    std::map<dsstring, EntityData>  m_nodes;

    void recurs_explore_entities( DrawSpace::Utils::JSONParser& p_parser, int& p_token_index, DrawSpace::Core::Entity* p_entity, DrawSpace::EntityGraph::EntityNode* p_entityNode, DrawSpace::EntityGraph::EntityNode* p_parentEntityNode, DrawSpace::Core::Aspect* p_aspect );

public:
    bool BuildFromFile( const std::string& p_filepath, DrawSpace::EntityGraph::EntityNode& p_node );

};
}
}

#endif