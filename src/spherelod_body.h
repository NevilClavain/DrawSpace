/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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
#include "runner.h"
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

protected:

    DrawSpace::SphericalLOD::Config*                                            m_config;

    
    dsreal                                                                      m_diameter;    
    dsreal                                                                      m_hotpoint_altitud;
    
    Face*                                                                       m_faces[6];

    std::vector<PatchUpdateHandler*>                                            m_patchupdate_handlers;

    int                                                                         m_current_face;    
    Patch*                                                                      m_current_patch;

    DrawSpace::Utils::TimeManager*                                              m_timemanager;

    void check_currentpatch_event( Patch* p_newvalue, int p_currentpatch_lod );

    static void build_meshe( long p_patch_resol, DrawSpace::Core::Meshe* p_meshe_dest, bool p_fastmode );

public:

    static DrawSpace::Core::Meshe*                                              m_planetpatch_meshe;
    static DrawSpace::Core::Meshe*                                              m_planetpatch2_meshe;


    Body( dsreal p_diameter, DrawSpace::Utils::TimeManager* p_time, DrawSpace::SphericalLOD::Config* p_config );
    virtual ~Body( void );

    static void BuildMeshe( void );
    static void BuildMesheHigh( void );
    
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

    dsreal GetDiameter( void );

};
}
}
#endif
