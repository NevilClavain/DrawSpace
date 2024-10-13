
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
#include "graphicobjects_helpers.h"

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

	// resources system event
	m_windowRenderingQueue->pushText({ m_resources_event, "CourierNew.10.spritefont", {255, 255, 255, 255}, {0, 120}, 0.0 });


	//////////////////////////////////////////////////////
	if (m_quadEntity2_state)
	{
		const auto quadEntity{ m_entitygraph.node("quadEntity2").data() };
		const auto& world_aspect{ quadEntity->aspectAccess(core::worldAspect::id) };
		const auto screenposition{ world_aspect.getComponent<core::maths::Real3Vector>("eg.std.projected_position")->getPurpose() };
		dataCloud->updateDataValue<maths::Real3Vector>("quadEntity2_projected_position", screenposition);
	}
	//////////////////////////////////////////////////////
	// 	
	// quadEntity0
	
	if (true == m_quadEntity0_state_request && false == m_quadEntity0_state)
	{
		// add quadEntity0

		dataCloud->registerData<maths::Real4Vector>("quad0_color");

		Entitygraph::Node& bufferRenderingNode{ m_entitygraph.node("bufferRenderingEntity") };

		auto& quadNode{ m_entitygraph.add(bufferRenderingNode, "quadEntity0") };
		const auto quadEntity{ quadNode.data() };


		auto& quad_resource_aspect{ quadEntity->makeAspect(core::resourcesAspect::id) };

		/////////// Add shaders

		quad_resource_aspect.addComponent<std::pair<std::string, Shader>>("vertexShader", std::make_pair("color_vs", Shader(vertexShader)));
		quad_resource_aspect.addComponent<std::pair<std::string, Shader>>("pixelShader", std::make_pair("color_ps", Shader(pixelShader)));

		/////////// Add linemeshe

		LineMeshe square;

		square.push(Vertex(-0.25, -0.5, 0.0));
		square.push(Vertex(0.25, -0.5, 0.0));
		square.push(Vertex(0.5, 0.5, 0.0));
		square.push(Vertex(-0.5, 0.5, 0.0));

		square.push({ 0, 1 });
		square.push({ 1, 2 });
		square.push({ 2, 3 });
		square.push({ 3, 0 });

		square.computeResourceUID();
		square.setSourceID("quadEntity0");

		square.setState(LineMeshe::State::BLOBLOADED);

		quad_resource_aspect.addComponent<LineMeshe>("line_square_0", square);



		auto& quad_rendering_aspect{ quadEntity->makeAspect(core::renderingAspect::id) };

		/////////// Add renderstate

		RenderState rs_noculling(RenderState::Operation::SETCULLING, "none");
		const std::vector<RenderState> rs_list = { rs_noculling };

		quad_rendering_aspect.addComponent<std::vector<RenderState>>("renderStates", rs_list);

		/////////// Draw lines

		rendering::DrawingControl lineDrawingControl;
		lineDrawingControl.pshaders_map.push_back(std::make_pair("quad0_color", "color"));


		quad_rendering_aspect.addComponent<rendering::DrawingControl>("squareRendering", lineDrawingControl);


		auto& quad_time_aspect{ quadEntity->makeAspect(core::timeAspect::id) };
		quad_time_aspect.addComponent<SyncVariable>("quad0_color", SyncVariable(SyncVariable::Type::POSITION, 0.5, SyncVariable::Direction::INC, 0.0, { 0.0, 1.0 }, SyncVariable::BoundariesManagement::MIRROR));

		quad_time_aspect.addComponent<SyncVariable>("y_rotation_angle", SyncVariable(SyncVariable::Type::ANGLE, 0.2, SyncVariable::Direction::DEC));

		/////////// World position

		auto& world_aspect{ quadEntity->makeAspect(core::worldAspect::id) };

		world_aspect.addComponent<transform::WorldPosition>("output");

		world_aspect.addComponent<transform::Animator>("animator_roty", transform::Animator(
			{
				{"yRotJointAnim.angle", "y_rotation_angle"},
				{"yRotJointAnim.output", "output"}
			},
			helpers::animators::makeYRotationJointAnimator()
		));


		world_aspect.addComponent<transform::Animator>("animator_positioning", transform::Animator
		(
			{},
			[](const core::ComponentContainer& p_world_aspect,
				const core::ComponentContainer& p_time_aspect,
				const transform::WorldPosition&,
				const std::unordered_map<std::string, std::string>&)
			{

				maths::Matrix positionmat;
				positionmat.translation(0.0, 2.0, 0.0);

				transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("output")->getPurpose() };
				wp.local_pos = wp.local_pos * positionmat;
			}
		));


		m_quadEntity0_state = true;
	}
	else if (false == m_quadEntity0_state_request && true == m_quadEntity0_state)
	{
		// remove quadEntity0

		dataCloud->removeData<maths::Real4Vector>("quad0_color");


		auto& quadNode{ m_entitygraph.node("quadEntity0") };
		m_entitygraph.remove(quadNode);

		m_quadEntity0_state = false;
	}
	
	//////////////////////////////////////////////////////
	// 	
	// quadEntity1

	if (true == m_quadEntity1_state_request && false == m_quadEntity1_state)
	{
		dataCloud->registerData<maths::Real4Vector>("quad1_color");

		Entitygraph::Node& bufferRenderingNode{ m_entitygraph.node("bufferRenderingEntity") };

		auto& quadNode{ m_entitygraph.add(bufferRenderingNode, "quadEntity1") };
		const auto quadEntity{ quadNode.data() };

		auto& quad_resource_aspect{ quadEntity->makeAspect(core::resourcesAspect::id) };

		/////////// Add shaders

		quad_resource_aspect.addComponent<std::pair<std::string, Shader>>("vertexShader", std::make_pair("color_vs", Shader(vertexShader)));
		quad_resource_aspect.addComponent<std::pair<std::string, Shader>>("pixelShader", std::make_pair("color_ps", Shader(pixelShader)));

		/////////// Add linemeshe

		LineMeshe square;

		square.push(Vertex(-0.5, -0.5, 0.0));
		square.push(Vertex(0.5, -0.5, 0.0));
		square.push(Vertex(0.5, 0.5, 0.0));
		square.push(Vertex(-0.5, 0.5, 0.0));

		square.push({ 0, 1 });
		square.push({ 1, 2 });
		square.push({ 2, 3 });
		square.push({ 3, 0 });

		square.computeResourceUID();
		square.setSourceID("quadEntity1");


		square.setState(LineMeshe::State::BLOBLOADED);

		quad_resource_aspect.addComponent<LineMeshe>("line_square_1", square);



		auto& quad_rendering_aspect{ quadEntity->makeAspect(core::renderingAspect::id) };

		/////////// Add renderstate

		RenderState rs_noculling(RenderState::Operation::SETCULLING, "none");
		const std::vector<RenderState> rs_list = { rs_noculling };

		quad_rendering_aspect.addComponent<std::vector<RenderState>>("renderStates", rs_list);

		/////////// Draw lines

		rendering::DrawingControl lineDrawingControl;
		lineDrawingControl.pshaders_map.push_back(std::make_pair("quad1_color", "color"));

		quad_rendering_aspect.addComponent<rendering::DrawingControl>("squareRendering", lineDrawingControl);




		auto& quad_time_aspect{ quadEntity->makeAspect(core::timeAspect::id) };
		quad_time_aspect.addComponent<SyncVariable>("quad1_color", SyncVariable(SyncVariable::Type::POSITION, 0.43, SyncVariable::Direction::INC, 0.0, { 0.0, 1.0 }, SyncVariable::BoundariesManagement::MIRROR));

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
				positionmat.translation(0.0, 0.0, -15.0);

				transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("position")->getPurpose() };
				wp.local_pos = wp.local_pos * positionmat;
			}
		));


		m_quadEntity1_state = true;
	}
	else if (false == m_quadEntity1_state_request && true == m_quadEntity1_state)
	{
		// remove quadEntity1

		dataCloud->removeData<maths::Real4Vector>("quad1_color");

		auto& quadNode{ m_entitygraph.node("quadEntity1") };
		m_entitygraph.remove(quadNode);


		m_quadEntity1_state = false;
	}
	
	//////////////////////////////////////////////////////
	// 	
	// quadEntity2

	if (true == m_quadEntity2_state_request && false == m_quadEntity2_state)
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
		square.push(Vertex(0.9, -0.5, 0.0, 1.0f, 1.0f));
		square.push(Vertex(0.9, 0.5, 0.0, 1.0f, 0.0f));
		square.push(Vertex(-0.9, 0.5, 0.0, 0.0f, 0.0f));

		const TrianglePrimitive<unsigned int> t1{ 0, 1, 2 };
		square.push(t1);

		const TrianglePrimitive<unsigned int> t2{ 0, 2, 3 };
		square.push(t2);

		square.computeNormales();
		square.computeTB();

		square.setState(TriangleMeshe::State::BLOBLOADED);

		square.computeResourceUID();
		square.setSourceID("quadEntity2");
		square.setSource(TriangleMeshe::Source::CONTENT_DYNAMIC_INIT);

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

		rendering::DrawingControl drawingControl;
		quad_rendering_aspect.addComponent<rendering::DrawingControl>("squareRendering", drawingControl);


		/////////// time aspect

		auto& quad_time_aspect{ quadEntity->makeAspect(core::timeAspect::id) };
		quad_time_aspect.addComponent<SyncVariable>("quad2_oscillation1", SyncVariable(SyncVariable::Type::POSITION, m_distribution(m_generator),
			SyncVariable::Direction::INC, 0.0, { 0.0, 1.0 }, SyncVariable::BoundariesManagement::MIRROR));


		quad_time_aspect.addComponent<SyncVariable>("quad2_oscillation2", SyncVariable(SyncVariable::Type::POSITION, m_distribution(m_generator),
			SyncVariable::Direction::INC, 0.0, { 0.0, 1.0 }, SyncVariable::BoundariesManagement::MIRROR));



		/////////// World position



		auto& world_aspect{ quadEntity->makeAspect(core::worldAspect::id) };

		world_aspect.addComponent<core::maths::Real3Vector>("eg.std.projected_position", core::maths::Real3Vector(0.0, 0.0, 0.0));
		world_aspect.addComponent<core::maths::Real3Vector>("eg.std.distance_to_camera", core::maths::Real3Vector(0.0, 0.0, 0.0));

		dataCloud->registerData<maths::Real3Vector>("quadEntity2_projected_position");


		world_aspect.addComponent<transform::WorldPosition>("position");

		core::maths::Quaternion quat1;
		quat1.rotationAxis(core::maths::Real3Vector(-0.5, 1.0, 0.0), 0.10);

		core::maths::Quaternion quat2;
		quat2.rotationAxis(core::maths::Real3Vector(1.0, -1.0, 0.0), 0.06);

		core::maths::Quaternion quat3;
		quat3.rotationAxis(core::maths::Real3Vector(0.0, 1.0, 0.9), 0.13);

		world_aspect.addComponent < core::maths::Quaternion>("quat1", quat1);
		world_aspect.addComponent < core::maths::Quaternion>("quat2", quat2);
		world_aspect.addComponent < core::maths::Quaternion>("quat3", quat3);

		world_aspect.addComponent<transform::Animator>("animator_osc", transform::Animator(
			{
				// input-output/components keys id mapping

				{"oscillationJointAnim.quat1", "quat1"},
				{"oscillationJointAnim.quat2", "quat2"},
				{"oscillationJointAnim.quat3", "quat3"},

				{"oscillationJointAnim.oscillation1", "quad2_oscillation1"},
				{"oscillationJointAnim.oscillation2", "quad2_oscillation2"},
				{"oscillationJointAnim.output", "position"}

			}, helpers::animators::makeOscillationJointAnimator()));

		world_aspect.addComponent<transform::Animator>("animator_positioning", transform::Animator
		(
			{},
			[](const core::ComponentContainer& p_world_aspect,
				const core::ComponentContainer& p_time_aspect,
				const transform::WorldPosition&,
				const std::unordered_map<std::string, std::string>&)
			{

				maths::Matrix positionmat;
				positionmat.translation(0.0, 0.0, -20.0);

				transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("position")->getPurpose() };
				wp.local_pos = wp.local_pos * positionmat;
			}
		));



		///// sprite : collimator
		
		const std::vector<rendering::RenderState> collimator_sprite_rs_list = { rs_noculling, rs_zbuffer, rs_fill, rs_texturepointsampling };
		const auto sprite_collimator{ helpers::plug2DSpriteWithPosition(m_entitygraph, "quadEntity2", "collimator_sprite", 0.025, 0.025, "sprite_vs", "sprite_ps", "target.bmp", collimator_sprite_rs_list, 1000) };
		
		// colimator text
		// TODO : 
		//	- update distance text
		
		helpers::plugTextWithPosition(m_entitygraph, "quadEntity2", "collimator_text", { "distance = ??? ", "CourierNew.10.spritefont", { 0, 200, 0, 255 }});


		m_quadEntity2_state = true;
		
	}
	else if (false == m_quadEntity2_state_request && true == m_quadEntity2_state)
	{

		auto& text_collimator_node{ m_entitygraph.node("collimator_text") };
		m_entitygraph.remove(text_collimator_node);


		auto& sprite_collimator_node { m_entitygraph.node("collimator_sprite") };
		m_entitygraph.remove(sprite_collimator_node);


		auto& quadNode{ m_entitygraph.node("quadEntity2") };
		m_entitygraph.remove(quadNode);

		dataCloud->removeData<maths::Real3Vector>("quadEntity2_projected_position");

		m_quadEntity2_state = false;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	if (m_quadEntity0_state)
	{
		auto& quadNode{ m_entitygraph.node("quadEntity0") };

		const auto quadEntity{ quadNode.data() };
		auto& quad_time_aspect{ quadEntity->aspectAccess(core::timeAspect::id) };

		renderMe::core::SyncVariable& mycolor_r{ quad_time_aspect.getComponent<renderMe::core::SyncVariable>("quad0_color")->getPurpose() };

		maths::Real4Vector mycolor;

		mycolor[0] = mycolor_r.value;
		mycolor[1] = mycolor_r.value;
		mycolor[2] = mycolor_r.value;
		mycolor[3] = 1.0;

		dataCloud->updateDataValue("quad0_color", mycolor);
	}


	if (m_quadEntity1_state)
	{
		auto& quadNode{ m_entitygraph.node("quadEntity1") };

		const auto quadEntity{ quadNode.data() };
		auto& quad_time_aspect{ quadEntity->aspectAccess(core::timeAspect::id) };

		renderMe::core::SyncVariable& mycolor_r{ quad_time_aspect.getComponent<renderMe::core::SyncVariable>("quad1_color")->getPurpose() };

		maths::Real4Vector mycolor;

		mycolor[0] = mycolor_r.value;
		mycolor[1] = mycolor_r.value;
		mycolor[2] = mycolor_r.value;
		mycolor[3] = 1.0;

		dataCloud->updateDataValue("quad1_color", mycolor);
	}


	if (m_quadEntity2_state)
	{
		auto& quadNode{ m_entitygraph.node("quadEntity2") };

		const auto quadEntity{ quadNode.data() };
		auto& quad_time_aspect{ quadEntity->aspectAccess(core::timeAspect::id) };

		renderMe::core::SyncVariable& osc1{ quad_time_aspect.getComponent<renderMe::core::SyncVariable>("quad2_oscillation1")->getPurpose() };
		if (osc1.value == 1.0)
		{
			osc1.step = m_distribution(m_generator);
		}
		else if (osc1.value == 0.0)
		{

			osc1.step = m_distribution(m_generator);
		}



		renderMe::core::SyncVariable& osc2{ quad_time_aspect.getComponent<renderMe::core::SyncVariable>("quad2_oscillation2")->getPurpose() };
		if (osc2.value == 1.0)
		{
			osc2.step = m_distribution(m_generator);
		}
		else if (osc2.value == 0.0)
		{
			osc2.step = m_distribution(m_generator);
		}
	}
}