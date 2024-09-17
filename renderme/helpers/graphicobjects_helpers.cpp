
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
							const double p_spriteHeight,
							const std::string& p_vshader,
							const std::string& p_pshader,
							const std::string& p_texture,
							const std::vector<rendering::RenderState>& p_renderstates_list,
							int p_rendering_order
						)
		{			
			auto& parentNodeNode{ p_entitygraph.node(p_parentid) };

			auto& sprite2DNode{ p_entitygraph.add(parentNodeNode, p_spriteEntityid) };
			const auto sprite2DEntity{ sprite2DNode.data() };

			auto& resource_aspect{ sprite2DEntity->makeAspect(core::resourcesAspect::id) };

			/////////// Add shaders

			resource_aspect.addComponent<std::pair<std::string, Shader>>("vertexShader", std::make_pair(p_vshader, Shader(vertexShader)));
			resource_aspect.addComponent<std::pair<std::string, Shader>>("pixelShader", std::make_pair(p_pshader, Shader(pixelShader)));

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
			resource_aspect.addComponent<std::pair<size_t, std::pair<std::string, Texture>>>("texture", std::make_pair(Texture::STAGE_0, std::make_pair(p_texture, Texture())));


			/////////// Add renderstate
			auto& rendering_aspect{ sprite2DEntity->makeAspect(core::renderingAspect::id) };

			rendering_aspect.addComponent<std::vector<rendering::RenderState>>("renderStates", p_renderstates_list);

			////////// Drawing control

			rendering::DrawingControl dc;
			rendering_aspect.addComponent<rendering::DrawingControl>("sprite2D_square", dc);

			rendering_aspect.addComponent<int>("renderingorder", p_rendering_order);

			/////////// time aspect

			auto& time_aspect{ sprite2DEntity->makeAspect(core::timeAspect::id) };

			/*
			time_aspect.addComponent<core::SyncVariable>("x_pos", core::SyncVariable(core::SyncVariable::Type::POSITION, 0.04, core::SyncVariable::Direction::INC));
			time_aspect.addComponent<core::SyncVariable>("y_pos", core::SyncVariable(core::SyncVariable::Type::POSITION, 0.01, core::SyncVariable::Direction::INC));
			*/

			time_aspect.addComponent<core::SyncVariable>("x_pos", core::SyncVariable(core::SyncVariable::Type::POSITION, 0.0));
			time_aspect.addComponent<core::SyncVariable>("y_pos", core::SyncVariable(core::SyncVariable::Type::POSITION, 0.0));


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

		core::SyncVariable& get2DSpriteXControl(renderMe::core::Entitygraph& p_entitygraph, const std::string& p_spriteEntityid)
		{
			auto& sprite2DNode{ p_entitygraph.node(p_spriteEntityid) };
			const auto sprite2DEntity{ sprite2DNode.data() };

			const auto& time_aspect{ sprite2DEntity->aspectAccess(core::timeAspect::id) };

			core::SyncVariable& x_pos{ time_aspect.getComponent< core::SyncVariable>("x_pos")->getPurpose()} ;
			return x_pos;
		}

		core::SyncVariable& get2DSpriteYControl(renderMe::core::Entitygraph& p_entitygraph, const std::string& p_spriteEntityid)
		{
			auto& sprite2DNode{ p_entitygraph.node(p_spriteEntityid) };
			const auto sprite2DEntity{ sprite2DNode.data() };

			const auto& time_aspect{ sprite2DEntity->aspectAccess(core::timeAspect::id) };

			core::SyncVariable& y_pos{ time_aspect.getComponent< core::SyncVariable>("y_pos")->getPurpose() };
			return y_pos;
		}
	}
}