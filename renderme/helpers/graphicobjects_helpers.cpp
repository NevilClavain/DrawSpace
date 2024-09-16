
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
#include "aspects.h"

#include "shader.h"
#include "trianglemeshe.h"
#include "renderstate.h"
#include "texture.h"
#include "renderingqueue.h"

#include "syncvariable.h"
#include "worldposition.h"
#include "animatorfunc.h"

#include "matrix.h"

namespace renderMe
{
	namespace helpers
	{
		void plug2DSprite(renderMe::core::Entitygraph& p_entitygraph, 
							const std::string& p_parentid, 
							const std::string& p_spriteEntityid,
							const double p_spriteWidth,
							const double p_spriteHeight
						)
		{			
			auto& parentNodeNode{ p_entitygraph.node(p_parentid) };

			auto& sprite2DNode{ p_entitygraph.add(parentNodeNode, p_spriteEntityid) };
			const auto sprite2DEntity{ sprite2DNode.data() };

			auto& resource_aspect{ sprite2DEntity->makeAspect(core::resourcesAspect::id) };

			/////////// Add shaders

			resource_aspect.addComponent<std::pair<std::string, Shader>>("vertexShader", std::make_pair("sprite_vs", Shader(vertexShader)));
			resource_aspect.addComponent<std::pair<std::string, Shader>>("pixelShader", std::make_pair("sprite_ps", Shader(pixelShader)));

			/////////// Add trianglemeshe
			TriangleMeshe sprite2D_square;


			sprite2D_square.push(Vertex(-p_spriteWidth / 2.0, -p_spriteHeight / 2.0, 0.0, 0.0f, 1.0f));
			sprite2D_square.push(Vertex(p_spriteWidth / 2.0, -p_spriteHeight / 2.0, 0.0, 1.0f, 1.0f));
			sprite2D_square.push(Vertex(p_spriteWidth / 2.0, p_spriteHeight / 2.0, 0.0, 1.0f, 0.0f));
			sprite2D_square.push(Vertex(-p_spriteWidth / 2.0, p_spriteHeight / 2.0, 0.0, 0.0f, 0.0f));

			const TrianglePrimitive<unsigned int> t1{ 0, 1, 2 };
			sprite2D_square.push(t1);

			const TrianglePrimitive<unsigned int> t2{ 0, 2, 3 };
			sprite2D_square.push(t2);

			sprite2D_square.computeResourceUID();
			sprite2D_square.setSourceID("sprite2DEntity");
			sprite2D_square.setSource(TriangleMeshe::Source::CONTENT_DYNAMIC_INIT);

			sprite2D_square.setState(TriangleMeshe::State::BLOBLOADED);

			resource_aspect.addComponent<TriangleMeshe>("sprite2D_square", sprite2D_square);

			/////////// Add texture
			resource_aspect.addComponent<std::pair<size_t, std::pair<std::string, Texture>>>("texture", std::make_pair(Texture::STAGE_0, std::make_pair("sb0.bmp", Texture())));


			/////////// Add renderstate
			auto& rendering_aspect{ sprite2DEntity->makeAspect(core::renderingAspect::id) };

			rendering::RenderState rs_noculling(rendering::RenderState::Operation::SETCULLING, "cw");
			rendering::RenderState rs_zbuffer(rendering::RenderState::Operation::ENABLEZBUFFER, "false");
			rendering::RenderState rs_fill(rendering::RenderState::Operation::SETFILLMODE, "solid");
			rendering::RenderState rs_texturepointsampling(rendering::RenderState::Operation::SETTEXTUREFILTERTYPE, "point");

			const std::vector<rendering::RenderState> rs_list = { rs_noculling, rs_zbuffer, rs_fill, rs_texturepointsampling };
			rendering_aspect.addComponent<std::vector<rendering::RenderState>>("renderStates", rs_list);

			////////// Drawing control

			rendering::DrawingControl dc;
			rendering_aspect.addComponent<rendering::DrawingControl>("sprite2D_square", dc);

			rendering_aspect.addComponent<int>("renderingorder", 1000);

			/////////// time aspect

			auto& time_aspect{ sprite2DEntity->makeAspect(core::timeAspect::id) };

			time_aspect.addComponent<core::SyncVariable>("x_pos", core::SyncVariable(core::SyncVariable::Type::POSITION, 0.04, core::SyncVariable::Direction::INC));
			time_aspect.addComponent<core::SyncVariable>("y_pos", core::SyncVariable(core::SyncVariable::Type::POSITION, 0.01, core::SyncVariable::Direction::INC));

			///////// world aspect

			auto& world_aspect{ sprite2DEntity->makeAspect(core::worldAspect::id) };
			world_aspect.addComponent<transform::WorldPosition>("position");

			world_aspect.addComponent<transform::Animator>("animator_positioning", transform::Animator
			(
				{},
				[](const core::ComponentContainer& p_world_aspect,
					const core::ComponentContainer& p_time_aspect,
					const transform::WorldPosition&,
					const std::unordered_map<std::string, std::string>&)
				{

					const auto& x_pos{ p_time_aspect.getComponent<core::SyncVariable>("x_pos")->getPurpose() };
					const auto& y_pos{ p_time_aspect.getComponent<core::SyncVariable>("y_pos")->getPurpose() };

					core::maths::Matrix positionmat;
					positionmat.translation(x_pos.value, y_pos.value, 0.0);

					transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("position")->getPurpose() };
					wp.local_pos = wp.local_pos * positionmat;
				}
			));

		}
	}
}