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

#ifndef _SPECTATORMOVEMENT_H_
#define _SPECTATORMOVEMENT_H_

#include "movement.h"
#include "inertbody.h"
#include "timemanager.h"
#include "scenenodegraph.h"

namespace DrawSpace
{
namespace Core
{
class SpectatorMovement : public Movement
{
protected:
    
    typedef DrawSpace::Core::CallBack<SpectatorMovement, void, DrawSpace::Utils::Timer*> SpectatorTimer;

    dsreal                          m_scalepos;
    long                            m_posperiod;

    bool                            m_linked_to_orbiter;

    DrawSpace::Dynamics::InertBody* m_attachedbody;

    bool                            m_compute;

    SpectatorTimer*                 m_timercb;

    dsstring                        m_name; // just to give a name to the timer entry

    DrawSpace::Utils::TimeManager*  m_time_manager;
    DrawSpace::Utils::Timer         m_timer;

    void compute_pos( void );

    void on_timer( DrawSpace::Utils::Timer* p_timer );

public:

    SpectatorMovement( void );
    virtual ~SpectatorMovement( void );

    void SetName( const dsstring& p_name );

    void Init( dsreal p_scalepos, long p_posperiod, bool p_orbiterlink );

    void SetRefBody( DrawSpace::Dynamics::InertBody* p_refbody );

    void Compute( DrawSpace::Utils::TimeManager& p_timemanager );

    dsreal GetScalePos( void );
    long GetPosPeriod( void );
    bool GetOrbiterLinkState( void );

    void Update( DrawSpace::Utils::TimeManager& p_timemanager );
    void Update2( DrawSpace::Utils::TimeManager& p_timemanager ) {};

    void GetBaseTransform( DrawSpace::Utils::Matrix& p_mat );
    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat ) { };
    
    void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node ) { };
    void OnUnregister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node ) { };

};

}
}

#endif
