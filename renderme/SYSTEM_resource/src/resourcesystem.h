
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
#include "system.h"
#include "logsink.h"
#include "logconf.h"
#include "logging.h"
#include "runner.h"


namespace renderMe
{
    namespace core { class Entity; }
    namespace core { class Entitygraph; }
   
    class ResourceSystem : public core::System
    {
    public:

        struct ShaderInfos
        {
            std::string name;
            
            std::string content;
            std::string contentMD5;
            size_t      contentSize{ 0 };

            bool        readyToUse{ false };
        };

        ResourceSystem(core::Entitygraph& p_entitygraph);
        ~ResourceSystem();

        void run();
        void killRunner();

    private:
        renderMe::core::logger::Sink        m_localLogger;
        renderMe::core::logger::Sink        m_localLoggerRunner;
        const std::string                   m_shadersBasePath{ "./shaders/resources" };
        const std::string                   m_shadersCachePath{ "./bc_cache" };

        //renderMe::core::Runner              m_runner;

        static constexpr unsigned int                           nbRunners{ 2 };
        std::vector<std::unique_ptr<renderMe::core::Runner>>    m_runner;
        int                                                     m_runnerIndex{ 0 };

        void handleShader(ShaderInfos& shaderInfos, int p_shaderType);
    };
}
