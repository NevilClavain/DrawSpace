
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

			Entitygraph() = default;

			// give an "unique" aspect
			Entitygraph(const Entitygraph&) = delete;
			Entitygraph(Entitygraph&&) = delete;
			Entitygraph& operator=(const Entitygraph& t) = delete;

			~Entitygraph() = default;

			Node&	makeRoot(Entity* p_entity);
			bool	hasRoot() const;

			st_tree::tree<core::Entity*>::df_pre_iterator preBegin();
			st_tree::tree<core::Entity*>::df_pre_iterator preEnd();

			st_tree::tree<core::Entity*>::df_post_iterator postBegin();
			st_tree::tree<core::Entity*>::df_post_iterator postEnd();


		private:
			st_tree::tree<core::Entity*> m_tree;
			
			
		};
	}
}