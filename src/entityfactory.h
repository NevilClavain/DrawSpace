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

    typedef DrawSpace::Core::CallBack2<Factory, void, const dsstring&, const dsstring&>                     ObjectContentEventCb;
    typedef DrawSpace::Core::CallBack2<Factory, void, const dsstring&, const dsstring&>                     ArrayContentEventCb;
    typedef DrawSpace::Core::CallBack3<Factory, void, const dsstring&, const dsstring&, const dsstring&>    StringContentEventCb;
    typedef DrawSpace::Core::CallBack3<Factory, void, const dsstring&, const dsstring&, dsreal>             NumericContentEventCb;


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

    ObjectContentEventCb            m_object_content_cb;
    ArrayContentEventCb             m_array_content_cb;
    StringContentEventCb            m_string_content_cb;
    NumericContentEventCb           m_num_content_cb;

    void on_object_content( const dsstring& p_owner_id, const dsstring& p_id );
    void on_array_content( const dsstring& p_owner_id, const dsstring& p_id );
    void on_string_content( const dsstring& p_owner_id, const dsstring& p_id, const dsstring& p_str );
    void on_num_content( const dsstring& p_owner_id, const dsstring& p_id, dsreal p_val );

public:
    Factory( void );
    bool BuildFromFile( const std::string& p_filepath, DrawSpace::EntityGraph::EntityNode& p_node );

};
}
}

#endif