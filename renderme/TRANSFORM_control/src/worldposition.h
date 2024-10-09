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
#include "matrix.h"

namespace renderMe
{
	namespace transform
	{
        struct WorldPosition
        {
            WorldPosition()
            {
                local_pos.identity();
                global_pos.identity();
            }

            WorldPosition(const core::maths::Matrix& p_local)
            {
                local_pos = p_local;
            }

            ~WorldPosition() = default;

            enum class TransformationComposition
            {
                TRANSFORMATION_RELATIVE_FROM_PARENT,
                TRANSFORMATION_ABSOLUTE,
                TRANSFORMATION_PARENT_PROJECTEDPOS 
            };

            core::maths::Matrix local_pos;
            core::maths::Matrix global_pos;

            bool                clip_it{ false };

            TransformationComposition composition_operation{ TransformationComposition::TRANSFORMATION_RELATIVE_FROM_PARENT };
        };
	}
}
