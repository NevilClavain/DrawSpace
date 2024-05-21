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

namespace renderMe
{
	namespace core
	{
        struct SyncVariable
        {
        public:
            enum class Type
            {
                ANGLE,
                POSITION
            };

            enum class Direction
            {
                INC,
                DEC
            };

            SyncVariable() = default;
            ~SyncVariable() = default;

            SyncVariable(Type p_type, double p_step, Direction p_direction = Direction::INC, double p_initial_value = 0.0) :
                type(p_type),
                step(p_step),
                direction(p_direction),
                value(p_initial_value)
            {
            };

            double      value{ 0.0 };
            double      step{ 0.0 };
            Direction   direction{ Direction::INC };
            Type        type;
        };
	}
}