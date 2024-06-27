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

#include <array>
namespace renderMe
{
    static constexpr int nbUVCoordsPerVertex = 4;

    struct Vertex
    {
    public:
        double	x{ 0.0 };
        double	y{ 0.0 };
        double	z{ 0.0 };

        double	nx{ 0.0 };
        double	ny{ 0.0 };
        double	nz{ 0.0 };

        double	tx{ 0.0 };
        double	ty{ 0.0 };
        double	tz{ 0.0 };

        double	bx{ 0.0 };
        double	by{ 0.0 };
        double	bz{ 0.0 };

        float	tu[nbUVCoordsPerVertex];
        float	tv[nbUVCoordsPerVertex];
        float	tw[nbUVCoordsPerVertex];
        float	ta[nbUVCoordsPerVertex];

        Vertex(void)
        {
            init_text_coords();
        };

        Vertex(double p_x, double p_y, double p_z) :
            x(p_x),
            y(p_y),
            z(p_z)
        {
            init_text_coords();
        }

        Vertex(double p_x, double p_y, double p_z, float p_tu0, float p_tv0) :
            x(p_x),
            y(p_y),
            z(p_z)
        {
            init_text_coords();

            tu[0] = p_tu0;
            tv[0] = p_tv0;
        }


    private:
        void init_text_coords(void)
        {
            for (long i = 0; i < nbUVCoordsPerVertex; i++)
            {
                tu[i] = 0.0;
                tv[i] = 0.0;
                tw[i] = 0.0;
                ta[i] = 0.0;
            }
        }
    };

    template<typename T>
    using TrianglePrimitive = std::array<T, 3>;

    template<typename T>
    using LinePrimitive = std::array<T, 2>;

} // renderMe
