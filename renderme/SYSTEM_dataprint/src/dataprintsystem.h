
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

#include <vector>
#include <string>
#include <set>
#include "system.h"


namespace mage
{
    //fwd decl
    namespace rendering
    {
        struct Queue;
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

        void setRenderingQueue(mage::rendering::Queue* p_queue);

        void addDatacloudFilter(const std::string& p_filter);

    private:

        static constexpr int                    dcNbCols{ 1 };
        static constexpr int                    dcNbRows{ 30 };
        static constexpr int                    dcColWidth{ 620 };
        static constexpr int                    dcRowHeight{ 18 };

        static constexpr int                    svNbCols{ 3 };
        static constexpr int                    svNbRows{ 10 };
        static constexpr int                    svColWidth{ 500 };
        static constexpr int                    svRowHeight{ 21 };

        static constexpr int                    rqNbCols{ 1 };
        static constexpr int                    rqNbRows{ 10 };
        static constexpr int                    rqColWidth{ 600 };
        static constexpr int                    rqRowHeight{ 21 };


        mage::rendering::Queue*             m_renderingQueue{ nullptr };

        std::vector<std::string>                m_dc_strings; // dataclouds display inputs

        std::vector<std::string>                m_sv_strings; // synchronized var display inputs

        std::vector<std::string>                m_rq_strings; // rendering queues display

        std::set<std::string>                   m_display_filters;

        void collectData();
        void print(const std::vector<std::string>& p_list, int p_x_base, int p_y_base, int p_nbCols, int p_nbRows, int p_colWidth, int p_rowHeight);

        static std::vector<std::string> splitString(const std::string& p_str, char p_delimiter);
        bool checkDcVar(const std::string& p_var_id) const;        
    };
}
