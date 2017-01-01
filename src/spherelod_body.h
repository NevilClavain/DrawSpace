/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _SPHERELOD_BODY_H_
#define _SPHERELOD_BODY_H_

#include "renderer.h"
#include "spherelod_face.h"
//#include "runner.h"
#include "timemanager.h"

namespace DrawSpace
{
namespace SphericalLOD
{
class Body
{
public:
    typedef DrawSpace::Core::BaseCallback2<void, Patch*, int>                                   PatchUpdateHandler;
    typedef DrawSpace::Core::CallBack<Body, void, DrawSpace::Utils::Timer*>                     BodyTimerCb;

    typedef enum
    {
        LOWRES_MESHE,
        AVGRES_MESHE,
        HIRES_MESHE,
        LOWRES_SKIRT_MESHE,
    
    } MesheType;

protected:

    DrawSpace::SphericalLOD::Config*                                            m_config;

    
    dsreal                                                                      m_diameter;    
    dsreal                                                                      m_hotpoint_altitud;
    
    Face*                                                                       m_faces[6];

    std::vector<PatchUpdateHandler*>                                            m_patchupdate_handlers;

    int                                                                         m_current_face;

    Patch*                                                                      m_current_patch;

    DrawSpace::Utils::TimeManager*                                              m_timemanager;

    double                                                                      m_relative_alt;

    bool                                                                        m_enable_cdlod;

    Utils::Vector                                                               m_invariant_viewerpos; //relatif au centre planete

    void check_currentpatch_event( Patch* p_newvalue, int p_currentpatch_lod );

    static void build_meshe( long p_patch_resol, DrawSpace::Core::Meshe* p_meshe_dest, bool p_fastmode, bool p_skirt );

public:

    static DrawSpace::Core::Meshe*                                              m_patch_meshe; //patch terrains
    static DrawSpace::Core::Meshe*                                              m_patch2_meshe; // patch terrains haute resolution
    static DrawSpace::Core::Meshe*                                              m_patch3_meshe; // patch terrains moyenne resolution
    static DrawSpace::Core::Meshe*                                              m_skirt_meshe; //les jupes terrains


    Body( DrawSpace::Utils::TimeManager* p_time, DrawSpace::SphericalLOD::Config* p_config, int p_layer_index,
            Patch::SubPassCreationHandler* p_handler, int p_nbLODRanges );

    virtual ~Body( void );

    static void BuildMeshes( void );

    
    void Compute( void );
    void Initialize( void );
    void RegisterPatchUpdateHandler( PatchUpdateHandler* p_handler );
    void UpdateHotPoint( const DrawSpace::Utils::Vector& p_hotpoint );
    DrawSpace::Core::Meshe* GetPatcheMeshe( void );

    Patch* GetFaceCurrentLeaf( int p_faceid );
    Face* GetFace( int p_faceid );

    dsreal GetHotPointAltitud( void );
    int GetCurrentFace (void );

    void SetHotState( bool p_hotstate );
    void UpdateRelativeAlt( dsreal p_alt );

    void UpdateInvariantViewerPos( const Utils::Vector& p_pos );
    void GetInvariantViewerPos( DrawSpace::Utils::Vector& p_pos );

    dsreal GetDiameter( void );

    void Reset( void ); // merge all

};
}
}
#endif
