
/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#include <unordered_map>
#include "st_tree.h"
#include "eventsource.h"

namespace renderMe
{
	namespace core
	{
		class Entity;

		enum class EntitygraphEvents
		{
			ENTITYGRAPHNODE_ADDED,
			ENTITYGRAPHNODE_REMOVED
		};

		class Entitygraph : public property::EventSource<EntitygraphEvents, const core::Entity&>
		{
		public:

			using Node			= st_tree::tree<core::Entity*>::node_type;
			using NodeIterator	= Node::iterator;

			using PreIterator	= st_tree::tree<core::Entity*>::df_pre_iterator;
			using PostIterator	= st_tree::tree<core::Entity*>::df_post_iterator;

			Entitygraph() = default;

			// give an "unique" aspect
			Entitygraph(const Entitygraph&) = delete;
			Entitygraph(Entitygraph&&) = delete;
			Entitygraph& operator=(const Entitygraph& t) = delete;

			~Entitygraph() = default;

			Node&			makeRoot(const std::string& p_entity_id);
			bool			hasRoot() const;

			Node&			add(Node& p_parent, const std::string& p_entity_id);

			void			remove(Node& p_node);

			Node&			node(const std::string& p_entity_id);


			PreIterator		preBegin();
			PreIterator		preEnd();

			PostIterator	postBegin();
			PostIterator	postEnd();

		private:
			st_tree::tree<core::Entity*>								m_tree;
			std::unordered_map<std::string, std::unique_ptr<Entity>>	m_entites;

			Node														m_rootNode;
			std::string													m_rootNodeName;

			std::unordered_map<std::string, NodeIterator>				m_nodes_iterator;					
		};
	}
}