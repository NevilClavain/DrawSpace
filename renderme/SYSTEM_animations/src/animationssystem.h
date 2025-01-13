
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
#include <map>
#include <string>
#include "system.h"


namespace renderMe
{
    namespace core { class Entity; }
    namespace core { class Entitygraph; }
    namespace core { class TimeMark; }
    namespace core { namespace maths { class Matrix; } }

    struct NodeAnimation;
    struct AnimationKeys;
    struct SceneNode;
   
    class AnimationsSystem : public core::System
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
