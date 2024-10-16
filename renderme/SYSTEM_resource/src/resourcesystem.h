
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
#include "json.h"


namespace renderMe
{
    // fwd decls
    namespace core { class Entity; }
    namespace core { class Entitygraph; }
    class Shader;
    class Texture;
    class TriangleMeshe;

    enum class ResourceSystemEvent
    {
        RESOURCE_SHADER_CACHE_CREATED,
        RESOURCE_SHADER_COMPILATION_BEGIN,
        RESOURCE_SHADER_COMPILATION_SUCCESS,
        RESOURCE_SHADER_COMPILATION_ERROR,
        RESOURCE_SHADER_LOAD_BEGIN,
        RESOURCE_SHADER_LOAD_SUCCESS,

        RESOURCE_TEXTURE_LOAD_BEGIN,
        RESOURCE_TEXTURE_LOAD_SUCCESS,

        RESOURCE_MESHE_LOAD_BEGIN,
        RESOURCE_MESHE_LOAD_SUCCESS

    };
   
    class ResourceSystem : public core::System, public renderMe::property::EventSource<ResourceSystemEvent, const std::string&>
    {
    public:

        ResourceSystem() = delete;
        ResourceSystem(core::Entitygraph& p_entitygraph);
        ~ResourceSystem();

        void run();
        void killRunner();

    private:
        renderMe::core::logger::Sink                            m_localLogger;
        renderMe::core::logger::Sink                            m_localLoggerRunner;
        const std::string                                       m_shadersBasePath{ "./shaders/resources" };
        const std::string                                       m_texturesBasePath{ "./textures" };
        const std::string                                       m_meshesBasePath{ "./meshes" };
        const std::string                                       m_shadersCachePath{ "./bc_cache" };

        renderMe::core::Json<Shader>::Callback	                m_jsonparser_cb;
        std::mutex                                              m_jsonparser_mutex;

        static constexpr unsigned int                           nbRunners{ 2 };
        std::vector<std::unique_ptr<renderMe::core::Runner>>    m_runner;
        int                                                     m_runnerIndex{ 0 };

        void handleShader(Shader& shaderInfos, int p_shaderType, const std::string& p_filename);
        void handleTexture(Texture& textureInfos, const std::string& p_filename);
        void handleTriangleMeshe(TriangleMeshe& mesheInfos, const std::string& p_filename);
    };
}
