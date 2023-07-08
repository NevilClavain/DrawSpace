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

#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Core
{
class Meshe;
}
}

struct FoliageConfig
{
    FoliageConfig(void) = default;
    // not copyable;
    FoliageConfig(const FoliageConfig&) = delete;
    FoliageConfig(FoliageConfig&&) = delete;

    dsstring                    foliages_meshes_paths;
    dsstring                    foliages_meshes_ids;
    bool                        foliages_global_lits;
    bool                        foliages_detailed_lits;
    int                         foliages_local_seeds;
    DrawSpace::Core::Meshe*     foliages_meshes{ nullptr };

    dsreal                      appearance;

    dsreal                      altitud_max;

    dsreal                      temperature_range_min;
    dsreal                      temperature_range_max;

    dsreal                      humidity_range_min;
    dsreal                      humidity_range_max;

    int                         nb_poles_min;
    int                         nb_poles_max;

    dsreal                      pole_ray_min;
    dsreal                      pole_ray_max;

    int                         nbpoints_per_pole_min;
    int                         nbpoints_per_pole_max;

};
