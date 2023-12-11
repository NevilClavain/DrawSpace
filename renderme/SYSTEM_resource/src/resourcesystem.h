
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

#include <mutex>

#include "system.h"
#include "logsink.h"
#include "logconf.h"
#include "logging.h"
#include "runner.h"
#include "eventsource.h"
#include "buffer.h"


namespace renderMe
{
    // fwd decls
    namespace core { class Entity; }
    namespace core { class Entitygraph; }
    class Shader;

    enum class ResourceSystemEvent
    {
        RESOURCE_SHADER_CACHE_CREATED,
        RESOURCE_SHADER_COMPILATION_BEGIN,
        RESOURCE_SHADER_COMPILATION_SUCCESS,
        RESOURCE_SHADER_COMPILATION_ERROR,
        RESOURCE_SHADER_LOAD_BEGIN,
        RESOURCE_SHADER_LOAD_SUCCESS,
        RESOURCE_SHADER_LOAD_ERROR,
    };
   
    class ResourceSystem : public core::System, public renderMe::property::EventSource<ResourceSystemEvent, const std::string&>
    {
    public:

        ResourceSystem(core::Entitygraph& p_entitygraph);
        ~ResourceSystem();

        void run();
        void killRunner();

    private:
        renderMe::core::logger::Sink        m_localLogger;
        renderMe::core::logger::Sink        m_localLoggerRunner;
        const std::string                   m_shadersBasePath{ "./shaders/resources" };
        const std::string                   m_shadersCachePath{ "./bc_cache" };


        static constexpr unsigned int                           nbRunners{ 2 };
        std::vector<std::unique_ptr<renderMe::core::Runner>>    m_runner;
        int                                                     m_runnerIndex{ 0 };

        void handleShader(Shader& shaderInfos, int p_shaderType);
    };
}
