/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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
    static const int        nbLODRangesMax              = 30;

    static const dsreal     detailsPatchScaling         = 1.0;

    static const int        lod0base                    = 19000.0;
    static const int        nbLODRanges_inertBodies     = 11;
    static const int        nbLODRanges_freeCameras     = 11;

    static const int        patchResolution             = 15;
    static const int        patchHighResolution         = 256;
    static const int        patchAvgResolution          = 256;

    static const dsreal     hotRelativeAlt              = 4.2;
    static const int        timerPeriod                 = 20;     // ms

    static const int        SurfaceLayer                = 0;
    static const int        AtmosphereLayer             = 1;
    static const int        FlatCloudsLayer             = 2;
    static const int        OceansLayer                 = 3;

}
}
