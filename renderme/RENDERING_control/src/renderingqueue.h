
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
#include <map>
#include <unordered_map>
#include <functional>
#include "tvector.h"
#include "matrix.h"
#include "renderstate.h"
#include "shader.h"

namespace renderMe
{
	namespace rendering
	{
		//fwd decl
		struct PrimitiveDrawing;	
		class RenderingQueueSystem;


		struct DrawingControl
		{
		public:

			DrawingControl()
			{
				world.identity();
				view.identity();
			}

			~DrawingControl() = default;

			core::maths::Matrix world;
			core::maths::Matrix view;
			core::maths::Matrix proj;

			bool				ready{ false };

			std::function<void()> setup{ [] {} };
			std::function<void()> teardown{ [] {} };

			//shaders params mapping description
			// dataCloud variable id / shader argument section id in shader json
			std::vector<std::pair<std::string, std::string>> vshaders_map;
			std::vector<std::pair<std::string, std::string>> pshaders_map;

			// shaders params to apply
			// dataCloud variable id/shader argument
			std::vector<std::pair<std::string, renderMe::Shader::Argument>> vshaders_map_cnx; // computed from vshaders_map and the queue current vshader
			std::vector<std::pair<std::string, renderMe::Shader::Argument>> pshaders_map_cnx; // computed from pshaders_map and the queue current pshader

			std::string owner_entity_id; // to be completed by queue system

		};

		class Queue
		{
		public:

			enum class Purpose
			{
				UNDEFINED,
				SCREEN_RENDERING,
				INTERMEDIATE_RENDERING
			};

			enum class State
			{
				WAIT_INIT,
				READY,
				ERROR_ORPHAN
			};

			struct Text
			{
				std::string							text;
				std::string							font;
				renderMe::core::maths::RGBAColor	color{ 255, 255, 255, 255 };
				renderMe::core::maths::IntCoords2D	position;
				float								rotation_rad{ 0.0 };
			};

			struct TriangleMeshePayload
			{
				// key = entity id
				std::unordered_map<std::string, DrawingControl> list;
			};

			struct LineMeshePayload
			{
				// key = entity id
				std::unordered_map<std::string, DrawingControl> list;
			};

			struct RenderStatePayload
			{
				// renderstates set
				std::vector<RenderState>								description;

				// key = triangleMeshe D3D11 id
				std::unordered_map<std::string, TriangleMeshePayload>	trianglemeshes_list;

				// key = lineMeshe D3D11 id
				std::unordered_map<std::string, LineMeshePayload>		linemeshes_list;
			};

			struct PixelShaderPayload
			{
				// key = renderstate set strings dump concatenation (RenderState::toString())
				std::unordered_map<std::string, RenderStatePayload> list;
			};

			struct VertexShaderPayload
			{
				// key = pixel shader D3D11 id
				std::unordered_map<std::string, PixelShaderPayload> list;
			};

			// key = vertex shader D3D11 id
			using QueueNodes = std::unordered_map<std::string, VertexShaderPayload>;

			////////////////////////////////////////////////////////////////////

			Queue(const std::string& p_name);
			~Queue() = default;

			std::string					getName() const;
			Purpose						getPurpose() const;
			State						getState() const;

			void						setState(State p_newstate);
			void						setPurpose(Purpose p_purpose);

			void						enableTargetClearing(bool p_enable);
			void						setTargetClearColor(const core::maths::RGBAColor& p_color);

			bool						getTargetClearing() const;
			core::maths::RGBAColor		getTargetClearColor() const;

			void						setText(int p_id, const Text& p_text);
			void						clearTexts();

			
			QueueNodes					getQueueNodes() const;
			void						setQueueNodes(const QueueNodes& p_nodes);
			

			const std::map<int, Text>&	texts() const;
		
		private:
			std::string						m_name;
			Purpose							m_purpose{ Purpose::UNDEFINED };
			State							m_state{ State::WAIT_INIT };
			bool							m_clear_target{ false };
			core::maths::RGBAColor			m_target_clear_color;
			std::map<int,Text>				m_texts;

			QueueNodes						m_queueNodes;
		};
	}
}
