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

#include "renderingqueue.h"
#include "component.h"
#include "exceptions.h"

using namespace renderMe::rendering;
using namespace renderMe::core;
using namespace renderMe::core::maths;

Queue::Queue(const std::string& p_name) :
m_name(p_name)
{}

std::string Queue::getName() const
{
	return m_name;
}

Queue::Purpose Queue::getPurpose() const
{
	return m_purpose;
}

Queue::State Queue::getState() const
{
	return m_state;
}

void Queue::setState(State p_newstate)
{
	m_state = p_newstate;
}

void Queue::setScreenRenderingPurpose()
{
	m_purpose = Purpose::SCREEN_RENDERING;
}

void Queue::setBufferRenderingPurpose(core::ComponentList<std::pair<size_t, renderMe::Texture>> p_textures_list)
{

	if (m_targetStage < p_textures_list.size())
	{
		const auto& render_target{ p_textures_list.at(m_targetStage)->getPurpose().second };
		m_targetTextureName = render_target.getName();
	}
	else
	{
		_EXCEPTION("Missing rendertarget texture on requested stage for BUFFER_RENDERING queue : " + getName());
	}

	m_purpose = Purpose::BUFFER_RENDERING;
}

void Queue::enableTargetClearing(bool p_enable)
{
	m_clear_target = true;
}

void Queue::setTargetClearColor(const RGBAColor& p_color)
{
	m_target_clear_color = p_color;
}

bool Queue::getTargetClearing() const
{
	return m_clear_target;
}

RGBAColor Queue::getTargetClearColor() const
{
	return m_target_clear_color;
}

void Queue::setText(int p_id, const Queue::Text& p_text)
{
	// can overwrite (to update text)
	m_texts[p_id] = p_text;
}

void Queue::clearTexts()
{
	m_texts.clear();
}

const std::map<int, Queue::Text>& Queue::texts() const
{
	return m_texts;
}

Queue::QueueNodes Queue::getQueueNodes() const
{
	return m_queueNodes;
}

void Queue::setQueueNodes(const Queue::QueueNodes& p_nodes)
{
	m_queueNodes = p_nodes;
}

void Queue::setCurrentView(const std::string& p_entityId)
{
	m_currentView = p_entityId;
}

std::string	Queue::getCurrentView() const
{
	return m_currentView;
}

std::string	Queue::getTargetTextureName() const
{
	return m_targetTextureName;
}

void Queue::setTargetStage(size_t p_stage)
{
	m_targetStage = p_stage;
}