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

#include "entitygraph.h"
#include "entity.h"
#include "exceptions.h"

using namespace renderMe::core;

Entitygraph::Node& Entitygraph::makeRoot(const std::string& p_entity_id)
{
	if (hasRoot())
	{
		_EXCEPTION("Entitygraph root already set")
	}

	m_entites[p_entity_id] = std::make_unique<Entity>(p_entity_id);
	m_tree.insert(m_entites.at(p_entity_id).get());

	for (const auto& call : m_callbacks)
	{
		call(EntitygraphEvents::ENTITYGRAPHNODE_ADDED, *m_entites.at(p_entity_id).get());
	}
	
	return m_tree.root();
}

bool Entitygraph::hasRoot() const
{
	return (m_tree.depth() > 0);
}

Entitygraph::Node& Entitygraph::add(Node& p_parent, const std::string& p_entity_id)
{
	const auto parent_id{ p_parent.data()->getId() };

	if (0 == m_entites.count(parent_id))
	{
		_EXCEPTION("parent not registered")
	}

	m_entites[p_entity_id] = std::make_unique<Entity>(p_entity_id);


	auto& new_node{ *(p_parent.insert(&*(m_entites[p_entity_id].get()))) };

	for (const auto& call : m_callbacks)
	{
		call(EntitygraphEvents::ENTITYGRAPHNODE_ADDED, *m_entites.at(p_entity_id).get());
	}

	return new_node;
}


st_tree::tree<Entity*>::df_pre_iterator Entitygraph::preBegin()
{
	return m_tree.df_pre_begin();
}

st_tree::tree<Entity*>::df_pre_iterator Entitygraph::preEnd()
{
	return m_tree.df_pre_end();
}

st_tree::tree<Entity*>::df_post_iterator Entitygraph::postBegin()
{
	return m_tree.df_post_begin();
}

st_tree::tree<Entity*>::df_post_iterator Entitygraph::postEnd()
{
	return m_tree.df_post_end();
}
