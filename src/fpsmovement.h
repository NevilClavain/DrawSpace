/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

#ifndef _FPSMOVEMENT_H_
#define _FPSMOVEMENT_H_

#include "movement.h"
#include "quaternion.h"
#include "scenenodegraph.h"

namespace DrawSpace
{
namespace Core
{
class FPSMovement// : public Movement
{
protected:

    DrawSpace::Utils::Matrix    m_result;

    Utils::Vector               m_local_speed;

	// les angles
	dsreal			            m_ayaw;
	dsreal			            m_apitch;

	// les quaternions
    Utils::Quaternion		    m_qyaw;
	Utils::Quaternion		    m_qpitch;

    Utils::Quaternion		    m_current_res;
		
	// les sorties
    Utils::Matrix			    m_position;
    Utils::Matrix			    m_orientation;

    bool                        m_ymvt;
   
public:

    FPSMovement( bool p_ymvt = false );
    virtual ~FPSMovement( void );

    void Init( const Utils::Vector& p_init_pos, dsreal p_initial_yaw = 0.0, dsreal p_initial_pitch = 0.0 );
    void InitRot( void );

    void RotateYaw( dsreal p_speed, Utils::TimeManager& p_timemanager );
    void RotatePitch( dsreal p_speed, Utils::TimeManager& p_timemanager );

    void SetSpeed( dsreal p_speed );
    void Compute( Utils::TimeManager& p_timemanager );

    void GetCurrentPos( Utils::Vector& p_pos );
    dsreal GetCurrentYaw( void );
    dsreal GetCurrentPitch( void );


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
