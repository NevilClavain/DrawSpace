
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

#include "module_impl.h"
#include <string>

#include "aspects.h"
#include "datacloud.h"
#include "sysengine.h"

#include "linemeshe.h"
#include "trianglemeshe.h"
#include "renderstate.h"

#include "syncvariable.h"

#include "worldposition.h"
#include "animatorfunc.h"
#include "animators_helpers.h"

using namespace mage;
using namespace mage::core;
using namespace mage::rendering;

void ModuleImpl::run(void)
{

	const auto dataCloud{ mage::rendering::Datacloud::getInstance() };

	/////////////////////////////////////////////////////

	auto sysEngine{ SystemEngine::getInstance() };
	sysEngine->run();
	
	//////////////////////////////////////////////////////
	// 	
	// quadEntity

	if (true == m_quadEntity_state_request && false == m_quadEntity_state)
	{
		Entitygraph::Node& bufferRenderingNode{ m_entitygraph.node("bufferRenderingEntity") };
		auto& quadNode{ m_entitygraph.add(bufferRenderingNode, "quadEntity2") };


		const auto quadEntity{ quadNode.data() };

		auto& quad_resource_aspect{ quadEntity->makeAspect(core::resourcesAspect::id) };

		/////////// Add shaders

		quad_resource_aspect.addComponent<std::pair<std::string, Shader>>("vertexShader", std::make_pair("texture_vs", Shader(vertexShader)));
		quad_resource_aspect.addComponent<std::pair<std::string, Shader>>("pixelShader", std::make_pair("texture_ps", Shader(pixelShader)));

		/////////// Add trianglemeshe
		TriangleMeshe square;

		square.push(Vertex(-0.9, -0.5, 0.0, 0.0f, 1.0f));
		square.push(Vertex(0.899, -0.5, 0.0, 1.0f, 1.0f));
		square.push(Vertex(0.9, 0.5, 0.0, 1.0f, 0.0f));
		square.push(Vertex(-0.9, 0.5, 0.0, 0.0f, 0.0f));

		const TrianglePrimitive<unsigned int> t1{ 0, 1, 2 };
		square.push(t1);

		const TrianglePrimitive<unsigned int> t2{ 0, 2, 3 };
		square.push(t2);

		square.computeNormales();
		square.computeTB();

		square.computeResourceUID();
		square.setSourceID("quadEntity2");
		square.setSource(TriangleMeshe::Source::CONTENT_DYNAMIC_INIT);

		square.setState(TriangleMeshe::State::BLOBLOADED);

		quad_resource_aspect.addComponent<TriangleMeshe>("square", square);

		/////////// Add texture

		quad_resource_aspect.addComponent<std::pair<size_t, std::pair<std::string, Texture>>>("texture", std::make_pair(Texture::STAGE_0, std::make_pair("map.jpg", Texture())));


		auto& quad_rendering_aspect{ quadEntity->makeAspect(core::renderingAspect::id) };

		/////////// Add renderstate

		RenderState rs_noculling(RenderState::Operation::SETCULLING, "none");
		RenderState rs_zbuffer(RenderState::Operation::ENABLEZBUFFER, "false");
		RenderState rs_fill(RenderState::Operation::SETFILLMODE, "solid");
		RenderState rs_texturepointsampling(RenderState::Operation::SETTEXTUREFILTERTYPE, "point");


		const std::vector<RenderState> rs_list = { rs_noculling, rs_zbuffer, rs_fill, rs_texturepointsampling };

		quad_rendering_aspect.addComponent<std::vector<RenderState>>("renderStates", rs_list);

		/////////// Draw triangles

		quad_rendering_aspect.addComponent<rendering::DrawingControl>("quad2", rendering::DrawingControl());


		/////////// time aspect

		auto& quad_time_aspect{ quadEntity->makeAspect(core::timeAspect::id) };

		/////////// World position



		auto& world_aspect{ quadEntity->makeAspect(core::worldAspect::id) };

		world_aspect.addComponent<transform::WorldPosition>("position");


		world_aspect.addComponent<transform::Animator>("animator_positioning", transform::Animator
		(
			{},
			[](const core::ComponentContainer& p_world_aspect,
				const core::ComponentContainer& p_time_aspect,
				const transform::WorldPosition&,
				const std::unordered_map<std::string, std::string>&)
			{

				maths::Matrix positionmat;
				positionmat.translation(0.0, 0.0, -5.60001);

				transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("position")->getPurpose() };
				wp.local_pos = wp.local_pos * positionmat;
			}
		));


		m_quadEntity_state = true;
	}
	else if (false == m_quadEntity_state_request && true == m_quadEntity_state)
	{
		// remove quadEntity2
		dataCloud->removeData<maths::Real4Vector>("quad2_color");


		auto& quadNode{ m_entitygraph.node("quadEntity2") };
		m_entitygraph.remove(quadNode);


		m_quadEntity_state = false;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}