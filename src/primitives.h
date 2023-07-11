/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of DrawSpace.
*
*    DrawSpace is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    DrawSpace is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#include <array>
#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Core
{
struct Vertex
{
private:
    void init_text_coords(void)
    {
        for (long i = 0; i < 9; i++)
        {
            tu[i] = 0.0;
            tv[i] = 0.0;
            tw[i] = 0.0;
            ta[i] = 0.0;
        }
    }
public:
    dsreal	x{ 0.0 };
    dsreal	y{ 0.0 };
    dsreal	z{ 0.0 };

    dsreal	nx{ 0.0 };
    dsreal	ny{ 0.0 };
    dsreal	nz{ 0.0 };

    dsreal	tx{ 0.0 };
    dsreal	ty{ 0.0 };
    dsreal	tz{ 0.0 };

    dsreal	bx{ 0.0 };
    dsreal	by{ 0.0 };
    dsreal	bz{ 0.0 };

    float	tu[9];
    float	tv[9];
    float	tw[9];
    float	ta[9];

    Vertex(void)
    {
        init_text_coords();
    };

    Vertex(dsreal p_x, dsreal p_y, dsreal p_z) :
        x(p_x),
        y(p_y),
        z(p_z)
    {
        init_text_coords();
    }
};

template<typename T>
using TrianglePrimitive = std::array<T,3>;

template<typename T>
using LinePrimitive = std::array<T, 2>;

}
}

