
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
#include "tvector.h"

namespace renderMe
{
	namespace rendering
	{
		//fwd decl
		struct PrimitiveDrawing;
		class RenderingQueueSystem;

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
			

			using DrawingNodes		= std::vector<PrimitiveDrawing*>;

			using TexturesIdsMap	= std::unordered_map<std::string, DrawingNodes>;
			using MeshesIdsMap		= std::unordered_map<std::string, TexturesIdsMap>;
			using ShaderIdsMap		= std::unordered_map<std::string, MeshesIdsMap>;

			using QueueNodes		= ShaderIdsMap;

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
