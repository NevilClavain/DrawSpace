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

#include <array>
namespace mage
{
    static constexpr int nbUVCoordsPerVertex = 9;

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
                /*
                tu[i] = -1.0;
                tv[i] = -1.0;
                tw[i] = -1.0;
                ta[i] = -1.0;
                */

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

} // mage
