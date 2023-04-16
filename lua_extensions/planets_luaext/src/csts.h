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

namespace LOD
{
namespace cst
{
    static constexpr int        nbLODRangesMax              = 30;

    static constexpr dsreal     detailsPatchScaling         = 1.0;

    static constexpr int        lod0base                    = 19000.0;
    static constexpr int        nbLODRanges_inertBodies     = 13;
    static constexpr int        nbLODRanges_freeCameras     = 14;

    static constexpr int        patchResolution             = 15;
    static constexpr int        patchHighResolution         = 256;
    static constexpr int        patchAvgResolution          = 256;

    static constexpr dsreal     hotRelativeAlt              = 4.2;
    static constexpr int        timerPeriod                 = 20;     // ms

    static constexpr int        SurfaceLayer                = 0;
    static constexpr int        AtmosphereLayer             = 1;
    static constexpr int        FlatCloudsLayer             = 2;
    static constexpr int        OceansLayer                 = 3;

    // heightmap generation
    static constexpr int        HeightMapRelativeLOD        = 4;

    // foliage
    static constexpr int        FoliageRootLODLevel         = 3;

    //static constexpr int heightmapTextureSize = 1024;
    static constexpr int heightmapTextureSize = 64;

}
}
