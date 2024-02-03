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
#include "singleton.h"
#include "componentcontainer.h"

namespace renderMe
{
    namespace rendering
    {
        class Datacloud : public property::Singleton<Datacloud>
        {
        public:
            Datacloud(void) = default;
            ~Datacloud() = default;

            template<typename T, class... Args>
            void registerData(const std::string& p_id, Args&&... p_args)
            {
                m_component_container.addComponent<T, Args...>(p_id, (std::forward<Args>(p_args))...);
            }

            template<typename T>
            T readDataValue(const std::string& p_id) const
            {
                const auto comp{ m_component_container.getComponent<T>(p_id) };
                if (nullptr == comp)
                {
                    _EXCEPTION("unknown data in datacloud: " + p_id);
                }
                return comp->getPurpose();
            }

            template<typename T>
            void updateData(const std::string& p_id, T value)
            {
                const auto comp{ m_component_container.getComponent<T>(p_id) };
                if (nullptr == comp)
                {
                    _EXCEPTION("unknown data in datacloud: " + p_id);
                }
                auto& purpose{ comp->getPurpose() };
                purpose = value;
            }

            template<typename T>
            void removeData(const std::string& p_id)
            {
                m_component_container.removeComponent<T>(p_id);
            }



        private:
            core::ComponentContainer m_component_container;


        };
    }
}