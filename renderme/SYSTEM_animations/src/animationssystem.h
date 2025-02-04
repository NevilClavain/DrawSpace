
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
#include <map>
#include <string>
#include "system.h"
#include "eventsource.h"

namespace mage
{
    namespace core { class Entity; }
    namespace core { class Entitygraph; }
    namespace core { class TimeMark; }
    namespace core { namespace maths { class Matrix; } }

    struct NodeAnimation;
    struct AnimationKeys;
    struct SceneNode;

    enum class AnimationSystemEvent
    {
        ANIMATION_START,
        ANIMATION_END,
    };
   
    class AnimationsSystem : public core::System, public mage::property::EventSource<AnimationSystemEvent, const std::string&, const std::string&>
    {
    public:
        AnimationsSystem() = delete;
        AnimationsSystem(core::Entitygraph& p_entitygraph);
        ~AnimationsSystem() = default;

        void run();

    private:
        void compute_node_animationresult_matrix(const NodeAnimation& p_node, double p_current_tick, core::maths::Matrix& p_out_matrix) const;
        bool animation_step(core::TimeMark& p_tmk, const AnimationKeys& p_animationkeys, std::map<std::string, SceneNode>& p_nodes);
    };
}
