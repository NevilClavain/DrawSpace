

/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#include <string>
#include <vector>
#include <utility>

namespace mage
{
	// fwd decl
	namespace core
	{
		class Entitygraph;

		namespace maths
		{
			class Matrix;
		}
	}

	namespace rendering
	{
		struct Queue;
	}

	namespace helpers
	{
		void logEntitygraph(core::Entitygraph& p_eg);

		void plugRenderingQuadView( mage::core::Entitygraph& p_entitygraph,
										float p_characteristics_v_width, float p_characteristics_v_height, 
										const std::string& p_parentid,
										const std::string& p_quadEntityid,
										const std::string& p_viewEntityid,
										mage::rendering::Queue* p_windowQueue,
										const std::string& p_vshader,
										const std::string& p_pshader,
										const std::vector<std::pair<size_t, Texture>>& p_renderTargets);

		void plugRenderingQueue( mage::core::Entitygraph& p_entitygraph, 
									const rendering::Queue& p_renderingqueue, 
									const std::string& p_parentid, const std::string& p_entityid);

		void plugView(mage::core::Entitygraph& p_entitygraph,
						const core::maths::Matrix& p_projection,
						const std::string& p_parentid, const std::string& p_entityid);
	}
}



