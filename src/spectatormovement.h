/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

namespace DrawSpace
{
namespace Core
{
class SpectatorMovement : public Movement
{
protected:

    typedef DrawSpace::Core::CallBack<SpectatorMovement, void, const dsstring&> SpectatorTimer;

    dsreal                          m_scalepos;
    long                            m_posperiod;

    bool                            m_linked_to_orbiter;

    DrawSpace::Dynamics::InertBody* m_attachedbody;

    bool                            m_compute;

    SpectatorTimer*                 m_timercb;

    dsstring                        m_name; // just to give a name to the timer entry

    void compute_pos( void );

    void on_timer( const dsstring& p_timername );

public:

    SpectatorMovement( void );
    virtual ~SpectatorMovement( void );

    void SetName( const dsstring& p_name );

    void Init( dsreal p_scalepos, long p_posperiod, bool p_orbiterlink );

    void SetRefBody( DrawSpace::Dynamics::InertBody* p_refbody );

    void Compute( DrawSpace::Utils::TimeManager& p_timemanager );
};

}
}

#endif