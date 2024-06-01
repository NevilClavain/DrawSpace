
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

using namespace renderMe;
using namespace renderMe::core;
using namespace renderMe::rendering;

void ModuleImpl::run(void)
{

	const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };

	/////////////////////////////////////////////////////

	auto sysEngine{ SystemEngine::getInstance() };
	sysEngine->run();

	/////////////////////////////////////////////////////

	//const auto currentFPS { m_timeInfos_time_aspect->getComponent<int>("framePerSeconds")->getPurpose() };
	const auto currentFPS{ dataCloud->readDataValue<long>("std.framesPerSecond") };

	const std::string fpsText{ std::string("fps = ") + std::to_string(currentFPS) };

	//m_windowRenderingQueue->setText(4, { fpsText, "CourierNew.10.spritefont", { 255, 0, 0, 255 }, { 0, 0 }, 0.0f });

	// resources system event
	m_windowRenderingQueue->setText(5, { m_resources_event, "CourierNew.10.spritefont", {255, 255, 255, 255}, {0, 120}, 0.0 });


	//////////////////////////////////////////////////////
	// 	
	// quadEntity

	if (true == m_quadEntity_state_request && false == m_quadEntity_state)
	{
		dataCloud->registerData<maths::Real4Vector>("quad2_color");
		dataCloud->updateDataValue< maths::Real4Vector>("quad2_color", maths::Real4Vector(0.43f, 0.0f, 0.0f, 1.0f));

		Entitygraph::Node& screenRenderingPassNode{ m_entitygraph.node("screenRenderingEntity") };

		auto& quadNode{ m_entitygraph.add(screenRenderingPassNode, "quadEntity2") };
		const auto quadEntity{ quadNode.data() };

		auto& quad_resource_aspect{ quadEntity->makeAspect(core::resourcesAspect::id) };

		/////////// Add shaders

		quad_resource_aspect.addComponent<Shader>("vertexShader", Shader("color_vs", 0));
		quad_resource_aspect.addComponent<Shader>("pixelShader", Shader("color_ps", 1));


		/////////// Add trianglemeshe
		TriangleMeshe square("square", TriangleMeshe::State::BLOBLOADED);

		square.push(Vertex(-m_characteristics_v_width / 2, -m_characteristics_v_height / 2, 0.0));
		square.push(Vertex(m_characteristics_v_width / 2, -m_characteristics_v_height / 2, 0.0));
		square.push(Vertex(m_characteristics_v_width / 2, m_characteristics_v_height / 2, 0.0));
		square.push(Vertex(-m_characteristics_v_width / 2, m_characteristics_v_height / 2, 0.0));

		const TrianglePrimitive<unsigned int> t1{ 0, 1, 2 };
		square.push(t1);

		const TrianglePrimitive<unsigned int> t2{ 0, 2, 3 };
		square.push(t2);

		square.computeNormales();
		square.computeTB();

		quad_resource_aspect.addComponent<TriangleMeshe>("square", square);

		auto& quad_rendering_aspect{ quadEntity->makeAspect(core::renderingAspect::id) };

		/////////// Add renderstate

		RenderState rs_noculling(RenderState::Operation::SETCULLING, "none");
		RenderState rs_zbuffer(RenderState::Operation::ENABLEZBUFFER, "false");
		RenderState rs_fill(RenderState::Operation::SETFILLMODE, "solid");

		const std::vector<RenderState> rs_list = { rs_noculling, rs_zbuffer, rs_fill };

		quad_rendering_aspect.addComponent<std::vector<RenderState>>("renderStates", rs_list);

		/////////// Draw triangles

		rendering::DrawingControl drawingControl;
		drawingControl.pshaders_map.push_back(std::make_pair("quad2_color", "color"));


		quad_rendering_aspect.addComponent<rendering::DrawingControl>("squareRendering", drawingControl);


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
				positionmat.translation(0.0, 0.0, -1.00001);

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