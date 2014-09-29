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

#ifndef _FPSMOVEMENT_H_
#define _FPSMOVEMENT_H_

#include "movement.h"
#include "quaternion.h"
#include "configurable.h"

#define FPSMVT_TEXT_KEYWORD           "FpsMvt"
#define FPSMVT_ARC_MAGICNUMBER        0x5045


namespace DrawSpace
{
namespace Core
{
class FPSMovement : public Movement, public Configurable
{
protected:
    Utils::Vector           m_local_speed;

	// les angles
	dsreal			        m_ayaw;
	dsreal			        m_apitch;

	// les quaternions
    Utils::Quaternion		m_qyaw;
	Utils::Quaternion		m_qpitch;

    Utils::Quaternion		m_current_res;
		
	// les sorties
    Utils::Matrix			m_position;
    Utils::Matrix			m_orientation;

    bool                    m_ymvt;

    bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:

    FPSMovement( bool p_ymvt = false );
    virtual ~FPSMovement( void );

    void Init( const Utils::Vector& p_init_pos, dsreal p_initial_yaw = 0.0, dsreal p_initial_pitch = 0.0 );
    void InitRot( void );

    void RotateYaw( dsreal p_speed, Utils::TimeManager& p_timemanager );
    void RotatePitch( dsreal p_speed, Utils::TimeManager& p_timemanager );

    void SetSpeed( dsreal p_speed );
    void Compute( Utils::TimeManager& p_timemanager /*, bool p_ymvt = false */ );

    void Serialize( Utils::Archive& p_archive  );
    bool Unserialize( Utils::Archive& p_archive );

    void DumpProperties( dsstring& p_text );
    bool ParseProperties( const dsstring& p_text );

    void ApplyProperties( void );

    static Configurable* Instanciate( void );


};
}
}

#endif