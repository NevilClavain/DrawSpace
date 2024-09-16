
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
#include <utility>

#include "entitygraph.h"
#include "entity.h"
#include "aspects.h"

#include "logsink.h"
#include "logconf.h"
#include "logging.h"

#include "renderingqueue.h"

#include "worldposition.h"
#include "matrix.h"

#include "shader.h"
#include "texture.h"
#include "trianglemeshe.h"
#include "renderstate.h"

#include "animatorfunc.h"

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

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void plugRenderingQuadView(renderMe::core::Entitygraph& p_entitygraph,
			                                float p_characteristics_v_width, float p_characteristics_v_height,
											const std::string& p_parentid,
											const std::string& p_quadEntityid,
											const std::string& p_viewEntityid,
											renderMe::rendering::Queue* p_windowQueue,
											const std::string& p_vshader,
											const std::string& p_pshader,
											const std::vector<std::pair<size_t, Texture>>& p_renderTargets
										)
		{
			/////////////// add viewpoint (camera) ////////////////////////

			auto& parentNodeNode{ p_entitygraph.node(p_parentid) };


			auto& viewPointNode{ p_entitygraph.add(parentNodeNode, p_viewEntityid) };
			const auto cameraEntity{ viewPointNode.data() };

			auto& camera_aspect{ cameraEntity->makeAspect(core::cameraAspect::id) };

			core::maths::Matrix projection;

			projection.perspective(p_characteristics_v_width, p_characteristics_v_height, 1.0, 10.00000000000);

			camera_aspect.addComponent<core::maths::Matrix>("projection", projection);

			auto& camera_world_aspect{ cameraEntity->makeAspect(core::worldAspect::id) };

			camera_world_aspect.addComponent<transform::WorldPosition>("camera_position", transform::WorldPosition());

			p_windowQueue->setCurrentView(p_viewEntityid);

			///////////////////////////////////////////////////////////////

			// add target quad aligned with screen

			auto& screenRenderingQuadNode{ p_entitygraph.add(parentNodeNode, p_quadEntityid) };
			const auto screenRenderingQuadEntity{ screenRenderingQuadNode.data() };


			/// RESOURCE ASPECT
			auto& quad_resource_aspect{ screenRenderingQuadEntity->makeAspect(core::resourcesAspect::id) };

			/////////// Add shaders

			quad_resource_aspect.addComponent<std::pair<std::string,Shader>>("vertexShader", std::make_pair(p_vshader, Shader(vertexShader)));
			quad_resource_aspect.addComponent<std::pair<std::string,Shader>>("pixelShader", std::make_pair(p_pshader, Shader(pixelShader)));

			/////////// Add trianglemeshe
			TriangleMeshe square;

			square.push(Vertex(-p_characteristics_v_width / 2, -p_characteristics_v_height / 2, 0.0, 0, 1));
			square.push(Vertex(p_characteristics_v_width / 2, -p_characteristics_v_height / 2, 0.0, 1, 1));
			square.push(Vertex(p_characteristics_v_width / 2, p_characteristics_v_height / 2, 0.0, 1, 0));
			square.push(Vertex(-p_characteristics_v_width / 2, p_characteristics_v_height / 2, 0.0, 0, 0));

			const TrianglePrimitive<unsigned int> t1{ 0, 1, 2 };
			square.push(t1);

			const TrianglePrimitive<unsigned int> t2{ 0, 2, 3 };
			square.push(t2);

			square.computeNormales();
			square.computeTB();

			square.computeResourceUID();
			square.setSourceID("helpers::plugRenderingQuadView");
			square.setSource(TriangleMeshe::Source::CONTENT_DYNAMIC_INIT);
			
			square.setState(TriangleMeshe::State::BLOBLOADED);

			quad_resource_aspect.addComponent<TriangleMeshe>("quad", square);

			/// RENDERING ASPECT
			auto& quad_rendering_aspect{ screenRenderingQuadEntity->makeAspect(core::renderingAspect::id) };

			quad_rendering_aspect.addComponent<core::renderingAspect::renderingTarget>("renderingTarget", core::renderingAspect::renderingTarget::BUFFER_RENDERINGTARGET);

			/////////// render target Texture

			const std::string texture_name_base{ "renderingquad_texture_" };

			for (size_t i = 0; i < p_renderTargets.size(); i++)
			{
				quad_resource_aspect.addComponent<std::pair<size_t, Texture>>(texture_name_base + std::to_string(p_renderTargets.at(i).first), p_renderTargets.at(i));
			}

			/////////// Add renderstate

			rendering::RenderState rs_noculling(rendering::RenderState::Operation::SETCULLING, "cw");
			rendering::RenderState rs_zbuffer(rendering::RenderState::Operation::ENABLEZBUFFER, "false");
			rendering::RenderState rs_fill(rendering::RenderState::Operation::SETFILLMODE, "solid");

			const std::vector<rendering::RenderState> rs_list = { rs_noculling, rs_zbuffer, rs_fill };

			quad_rendering_aspect.addComponent<std::vector<rendering::RenderState>>("renderStates", rs_list);


			/////////// Draw triangles

			rendering::DrawingControl drawingControl;
			quad_rendering_aspect.addComponent<rendering::DrawingControl>("screenRenderingQuad", drawingControl);


			/////////// time aspect
			// required for animator !

			screenRenderingQuadEntity->makeAspect(core::timeAspect::id);

			/////////// World position

			auto& world_aspect{ screenRenderingQuadEntity->makeAspect(core::worldAspect::id) };

			world_aspect.addComponent<transform::WorldPosition>("position");


			world_aspect.addComponent<transform::Animator>("animator_positioning", transform::Animator
			(
				{},
				[](const core::ComponentContainer& p_world_aspect,
					const core::ComponentContainer& p_time_aspect,
					const transform::WorldPosition&,
					const std::unordered_map<std::string, std::string>&)
				{

					core::maths::Matrix positionmat;
					positionmat.translation(0.0, 0.0, -1.00001);

					transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("position")->getPurpose() };
					wp.local_pos = wp.local_pos * positionmat;
				}
			));
		}



		///////////////////////////////////////////////////////////////////////////////////////////////////////////


		void plugRenderingQueue(renderMe::core::Entitygraph& p_entitygraph, const rendering::Queue& p_renderingqueue, const std::string& p_parentid, const std::string& p_entityid)
		{
			core::Entitygraph::Node& parentNode{ p_entitygraph.node(p_parentid) };
			auto& renderingQueueNode{ p_entitygraph.add(parentNode, p_entityid) };
			const auto renderingQueueNodeEntity{ renderingQueueNode.data() };

			auto& renderingAspect{ renderingQueueNodeEntity->makeAspect(core::renderingAspect::id) };

			renderingAspect.addComponent<rendering::Queue>("renderingQueue", p_renderingqueue);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void plugView(renderMe::core::Entitygraph& p_entitygraph,
			const core::maths::Matrix& p_projection,
			const std::string& p_parentid, const std::string& p_entityid)
		{
			core::Entitygraph::Node& bufferRenderingQueueNode{ p_entitygraph.node(p_parentid) };
			auto& viewPointNode{ p_entitygraph.add(bufferRenderingQueueNode, p_entityid) };
			const auto cameraEntity{ viewPointNode.data() };

			auto& camera_aspect{ cameraEntity->makeAspect(core::cameraAspect::id) };

			camera_aspect.addComponent<core::maths::Matrix>("projection", p_projection);
			auto& camera_world_aspect{ cameraEntity->makeAspect(core::worldAspect::id) };

			camera_world_aspect.addComponent<transform::WorldPosition>("camera_position", transform::WorldPosition());
		}
	}
}

