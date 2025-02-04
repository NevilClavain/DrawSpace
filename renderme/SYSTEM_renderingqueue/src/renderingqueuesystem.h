
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

#include <unordered_set>
#include "system.h"
#include "logsink.h"
#include "logconf.h"
#include "logging.h"
#include "eventsource.h"

namespace mage
{
    namespace core { class Entity; }
    namespace core { class Entitygraph; }
    namespace core { class ComponentContainer; }
    namespace rendering { struct Queue; }

    enum class RenderingQueueSystemEvent
    {
        LINEDRAWING_ADDED,
        LINEDRAWING_REMOVED,
        TRIANGLEDRAWING_ADDED,
        TRIANGLEDRAWING_REMOVED
    };

    class RenderingQueueSystem : public core::System, public mage::property::EventSource<RenderingQueueSystemEvent, const std::string&>
    {
    public:

        RenderingQueueSystem() = delete;
        RenderingQueueSystem(core::Entitygraph& p_entitygraph);
        ~RenderingQueueSystem() = default;

        void run();

        void requestRenderingqueueLogging(const std::string& p_entityid);

    private:

        mutable mage::core::logger::Sink        m_localLogger;

        std::unordered_set<std::string>             m_queuesToLog;

        void manageRenderingQueue();
        void handleRenderingQueuesState(core::Entity* p_entity, rendering::Queue& p_renderingQueue);

        void checkEntityInsertion(
                                    const std::string& p_entity_id, 
                                    const mage::core::ComponentContainer& p_resourceAspect,
                                    const mage::core::ComponentContainer& p_renderingAspect, 
                                    mage::rendering::Queue& p_renderingQueue
                                );

        void removeFromRenderingQueue(
                                    const std::string& p_entity_id, 
                                    mage::rendering::Queue& p_renderingQueue
                                );


        void logRenderingqueue(const std::string& p_entity_id, mage::rendering::Queue& p_renderingQueue) const;

    };
}