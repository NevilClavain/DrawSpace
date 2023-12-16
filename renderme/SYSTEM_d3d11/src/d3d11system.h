
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

#include <unordered_set>
#include "system.h"
#include "shaders_service.h"
#include "runner.h"



namespace renderMe
{   
    namespace core { class Entity; }
    namespace core { class Entitygraph; }
    namespace rendering { class Queue; }
    class Shader;
               
    class D3D11System : public core::System
    {
    public:

        D3D11System(core::Entitygraph& p_entitygraph);
        ~D3D11System() = default;

        void run();
        void killRunner();

        auto getServiceInvocationCallback() const
        {
            return m_service_invocation_cb;
        };

    private:
        bool	                                                m_initialized{ false };
        core::services::ShadersCompilationService::Callback     m_service_invocation_cb;

        renderMe::core::Runner                                  m_runner;

        void    manageInitialization();
        void    manageRenderingQueue() const;

        void    manageResources();

        void    handleShader(Shader& shaderInfos, int p_shaderType);

        static void handleRenderingQueuesState(core::Entity* p_entity, rendering::Queue& p_renderingQueue );
        static void renderQueue(rendering::Queue& p_renderingQueue);
    };
}
