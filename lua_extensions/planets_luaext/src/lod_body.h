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


#include "renderer.h"
#include "lod_face.h"
#include "timemanager.h"

namespace LOD
{
class Face;
struct Config;

class Body
{
public:
    using BodyTimerCb = DrawSpace::Core::CallBack<Body, void, DrawSpace::Timer*>;

    enum class MesheType
    {
        LOWRES_MESHE,
        AVGRES_MESHE,
        HIRES_MESHE,
        LOWRES_SKIRT_MESHE,    
    };

    static DrawSpace::Core::Meshe*                                              m_patch_meshe; //patch terrains
    static DrawSpace::Core::Meshe*                                              m_patch2_meshe; // patch terrains haute resolution
    static DrawSpace::Core::Meshe*                                              m_patch3_meshe; // patch terrains moyenne resolution
    static DrawSpace::Core::Meshe*                                              m_skirt_meshe; //les jupes terrains


    Body( Config* p_config, int p_layer_index, SubPass::SubPassCreationHandler* p_handler, int p_nbLODRanges, const dsstring& p_description );

    virtual ~Body( void );

    
    Patch*                          GetFaceCurrentLeaf(int p_faceid) const;
    Face*                           GetFace(int p_faceid) const;
    dsreal                          GetHotPointAltitud(void) const;
    int                             GetCurrentFace(void) const;
    void                            GetInvariantViewerPos(DrawSpace::Maths::Vector& p_pos) const;
    dsreal                          GetDiameter(void) const;
    dsstring                        GetDescription(void) const;
    dsreal                          GetRelativeAlt(void) const;

    
    void                            Initialize(void);
    void                            Compute( void );
    
    void                            UpdateHotPoint( const DrawSpace::Maths::Vector& p_hotpoint );
    void                            UpdateRelativeAlt(dsreal p_alt);
    void                            UpdateInvariantViewerPos(const DrawSpace::Maths::Vector& p_pos);

    void                            SetHotState( bool p_hotstate );
    
    void                            Reset( void ); // merge all

    static void                     BuildMeshes(void);
    static void                     DestroyMeshes(void);
    static DrawSpace::Core::Meshe*  GetPatcheMeshe(void);

private:

    Config* m_config{ nullptr };

    dsreal                                                                      m_diameter;
    dsreal                                                                      m_hotpoint_altitud;

    Face* m_faces[6];

    int                                                                         m_current_face{ -1 };
    double                                                                      m_relative_alt{ 0.0 };

    bool                                                                        m_enable_cdlod;

    DrawSpace::Maths::Vector                                                    m_invariant_viewerpos; //relatif au centre planete

    dsstring                                                                    m_description; // for debug purpose only;

    static void build_meshe(long p_patch_resol, DrawSpace::Core::Meshe* p_meshe_dest, bool p_fastmode, bool p_skirt);
};
}
