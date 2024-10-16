
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

#include <string>
#include <vector>

namespace renderMe
{
	// fwd decl
	namespace core
	{
		class Entity;
		class Entitygraph;
		struct SyncVariable;
	}

	namespace helpers
	{
		core::Entity* plug2DSpriteWithSyncVariables(renderMe::core::Entitygraph& p_entitygraph,
							const std::string& p_parentid,
							const std::string& p_spriteEntityid, 
							const double p_spriteWidth,
							const double p_spriteHeight,
							const std::string& p_vshader,
							const std::string& p_pshader,
							const std::string& p_texture,
							const std::vector<rendering::RenderState>& p_renderstates_list,
							int p_rendering_order
						);

		core::SyncVariable& getXPosSync(renderMe::core::Entitygraph& p_entitygraph, const std::string& p_spriteEntityid);
		core::SyncVariable& getYPosSync(renderMe::core::Entitygraph& p_entitygraph, const std::string& p_spriteEntityid);
		core::SyncVariable& getZRotSync(renderMe::core::Entitygraph& p_entitygraph, const std::string& p_spriteEntityid);

		core::Entity* plug2DSpriteWithPosition(renderMe::core::Entitygraph& p_entitygraph,
			const std::string& p_parentid,
			const std::string& p_spriteEntityid,
			const double p_spriteWidth,
			const double p_spriteHeight,
			const std::string& p_vshader,
			const std::string& p_pshader,
			const std::string& p_texture,
			const std::vector<rendering::RenderState>& p_renderstates_list,
			int p_rendering_order,
			float p_xpos = 0,
			float p_ypos = 0,
			float p_rot_radians = 0
		);


		core::Entity* plugTextWithPosition(renderMe::core::Entitygraph& p_entitygraph,
			const std::string& p_parentid,
			const std::string& p_textEntityid,
			const renderMe::rendering::Queue::Text& p_queue_text,
			float p_xpos = 0,
			float p_ypos = 0);


		double& getXPos(renderMe::core::Entitygraph& p_entitygraph, const std::string& p_spriteEntityid);
		double& getYPos(renderMe::core::Entitygraph& p_entitygraph, const std::string& p_spriteEntityid);
		double& getZRot(renderMe::core::Entitygraph& p_entitygraph, const std::string& p_spriteEntityid);

	}
}