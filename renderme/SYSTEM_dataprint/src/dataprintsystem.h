
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

#include <vector>
#include <string>
#include "system.h"


namespace renderMe
{
    //fwd decl
    namespace rendering
    {
        class Queue;
    }

    namespace core { class Entity; }
    namespace core { class Entitygraph; }
   
    class DataPrintSystem : public core::System
    {
    public:
        DataPrintSystem() = delete;
        DataPrintSystem(core::Entitygraph& p_entitygraph);
        ~DataPrintSystem() = default;

        void run();

        void setRenderingQueue(renderMe::rendering::Queue* p_queue);

    private:

        static constexpr int                    dcTextsIdBase{ 2000 };
        static constexpr int                    svTextsIdBase{ 3000 };

        static constexpr int                    dcNbCols{ 4 };
        static constexpr int                    dcNbRows{ 10 };

        static constexpr int                    dcColWidth{ 500 };
        static constexpr int                    dcRowHeight{ 25 };

        static constexpr int                    svNbCols{ 9 };
        static constexpr int                    svNbRows{ 10 };

        static constexpr int                    svColWidth{ 200 };
        static constexpr int                    svRowHeight{ 25 };

        renderMe::rendering::Queue*             m_renderingQueue{ nullptr };

        std::vector<std::string>                m_dc_strings; // dataclouds display inputs

        std::vector<std::string>                m_sv_strings; // synchronized varq display inputs

        void collectData();
        void print(const std::vector<std::string>& p_list, int p_y_base, int p_id_base, int p_nbCols, int p_nbRows, int p_colWidth, int p_rowHeight);

        
    };
}
