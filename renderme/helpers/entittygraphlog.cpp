
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

#include <map>
#include <string>
#include <functional>

#include "entitygraph.h"
#include "entity.h"
#include "aspects.h"

#include "logsink.h"
#include "logconf.h"
#include "logging.h"


extern renderMe::core::logger::Sink localLogger("Helpers", renderMe::core::logger::Configuration::getInstance());

namespace renderMe
{
	namespace helpers
	{
		void logEntitygraph(core::Entitygraph& p_eg)
		{

			_RENDERME_DEBUG(localLogger, ">>>>>>>>>>>>>>> ENTITY GRAPH DUMP BEGIN <<<<<<<<<<<<<<<<<<<<<<<<");

			struct ENode
			{
				std::string					 id;
				std::map<std::string, ENode> children;
			};

			ENode root;

			// build node tree that will be dumped to log
			for (auto it = p_eg.preBegin(); it != p_eg.preEnd(); ++it)
			{
				const renderMe::core::Entity* current_entity { it->data() };
				const std::string currId{ current_entity->getId() };

				const std::function<void(ENode&, const std::string&, const std::string&)> search
				{
					[&](ENode& p_node, const std::string& p_parentId, const std::string& p_id)
					{
						if (p_node.id == p_parentId)
						{
							ENode child;
							child.id = p_id;
							// found parent
							p_node.children[p_id] = child;
						}

						for (auto& e : p_node.children)
						{
							search(e.second, p_parentId, p_id);
						}						
					}
				};

				const auto parent_entity{ current_entity->getParent() };
				if (parent_entity)
				{
					const std::string parentId{ parent_entity->getId() };
					search(root, parentId, currId);
				}
				else
				{
					// create root;
					root.id = currId;
				}
			}

			// dump to log the built node tree

			const std::function<void(const ENode&, int)> logMe
			{
				[&](const ENode& p_node, int depth)
				{
					const auto& eg_node { p_eg.node(p_node.id) };

					const core::Entity* curr_entity{ eg_node.data() };
					
					std::string logstr;

					// entity name
					for (int i = 0; i < depth; i++)
					{
						logstr += "\t";
					}
					logstr += p_node.id;

					// aspects of this entity
					const std::map<int, std::string> aspects_translate
					{
						{ core::teapotAspect::id,		"teapotAspect" },
						{ core::renderingAspect::id,	"renderingAspect" },
						{ core::timeAspect::id,			"timeAspect" },
						{ core::resourcesAspect::id,	"resourcesAspect" },
						{ core::cameraAspect::id,		"cameraAspect" },
						{ core::worldAspect::id,		"worldAspect" },
					};

					for (const auto& e : aspects_translate)
					{
						if (curr_entity->hasAspect(e.first))
						{
							// log aspect name

							logstr += "\n";
							for (int i = 0; i < depth + 1; i++)
							{
								logstr += "\t";
							}

							logstr += e.second;

							/////////////////////
							const auto& cc{ curr_entity->aspectAccess(e.first) };
							const std::unordered_map<std::string, std::string>& comp_id_list{ cc.getComponentsIdWithTypeStrList() };

							for (const auto& c : comp_id_list)
							{
								logstr += "\n";
								for (int i = 0; i < depth + 2; i++)
								{
									logstr += "\t";
								}
								logstr += c.first;
								logstr += " ";
								logstr += c.second;												
							}
						}
					}


					_RENDERME_DEBUG(localLogger, logstr);

					for (auto& e : p_node.children)
					{
						logMe(e.second, depth+1);
					}
				}
			};

			logMe(root, 0);	

			_RENDERME_DEBUG(localLogger, ">>>>>>>>>>>>>>> ENTITY GRAPH DUMP END <<<<<<<<<<<<<<<<<<<<<<<<");
		}
	}
}

