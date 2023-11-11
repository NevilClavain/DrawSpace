
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



namespace renderMe
{
    namespace core { class Entity; }
    namespace core { class Entitygraph; }
   
    class ResourceSystem : public core::System
    {
    public:

        ResourceSystem(core::Entitygraph& p_entitygraph);
        ~ResourceSystem() = default;

        void run();

    private:
        renderMe::core::logger::Sink m_localLogger;


    };
}
