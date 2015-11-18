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

    typedef DrawSpace::Core::CallBack<Body, void, DrawSpace::Core::PropertyPool*>               RunnerMsgCb;
    typedef DrawSpace::Core::CallBack<Body, void, Core::Runner::State>                          RunnerEvtCb;

    typedef DrawSpace::Core::CallBack<Body, void, DrawSpace::Utils::Timer*>                     BodyTimerCb;

protected:

    DrawSpace::Core::Runner*                                                    m_runner;
    RunnerMsgCb*                                                                m_runnercb;
    RunnerEvtCb*                                                                m_runnerevt;

    DrawSpace::Utils::Timer                                                     m_timer;
    BodyTimerCb*                                                                m_timercb;

    Face*                                                                       m_faces[6];
    static DrawSpace::Core::Meshe*                                              m_planetpatch_meshe;

    dsreal                                                                      m_diameter;    
    dsreal                                                                      m_hotpoint_altitud;


    std::vector<PatchUpdateHandler*>                                            m_patchupdate_handlers;

    int                                                                         m_current_face;    
    Patch*                                                                      m_current_patch;

    DrawSpace::Utils::TimeManager*                                              m_timemanager;

    void check_currentpatch_event( Patch* p_newvalue, int p_currentpatch_lod );

    void on_runner_request( DrawSpace::Core::PropertyPool* p_args );
    void on_runner_result( DrawSpace::Core::Runner::State p_runnerstate );
    void on_timer( DrawSpace::Utils::Timer* p_timer );


public:

    Body( dsreal p_diameter, DrawSpace::Utils::TimeManager* p_time );
    virtual ~Body( void );

    static void BuildMeshe( void );
    
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

    friend class Drawing;    
};
}
}
#endif
