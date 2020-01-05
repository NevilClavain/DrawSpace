/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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

#ifndef _LONGLATMOVEMENT_H_
#define _LONGLATMOVEMENT_H_

#include "movement.h"
#include "quaternion.h"
#include "scenenodegraph.h"

namespace DrawSpace
{
namespace Core
{
class LongLatMovement// : public Movement
{
protected:

    DrawSpace::Utils::Matrix    m_result;

    dsreal                      m_longitud_theta;
    dsreal                      m_latitud_phi;

    dsreal                      m_longitud_theta_base;
    dsreal                      m_latitud_phi_base;


    dsreal                      m_alt;

    // camera orientation control
    dsreal                      m_current_theta;
    dsreal                      m_current_phi;
   
    Utils::Quaternion		    m_qyaw;
	Utils::Quaternion		    m_qpitch;
    Utils::Quaternion		    m_rot_res;

public:

    LongLatMovement( void );
    virtual ~LongLatMovement( void );

    void Init( dsreal p_init_longitud_theta, dsreal p_init_latitud_phi, dsreal p_init_alt, dsreal p_init_theta, dsreal p_init_phi );
    virtual void Compute( Utils::TimeManager& p_timemanager );

    void SetTheta( dsreal p_theta );
    void SetPhi( dsreal p_phi );
    void SetLatitud( dsreal p_latitud );
    void SetLongitud( dsreal p_longitud );
    void SetLatitudBase( dsreal p_latitud );
    void SetLongitudBase( dsreal p_longitud );
    void SetAlt( dsreal p_altitud );


    dsreal GetCurrentLongitud( void );
    dsreal GetCurrentLatitud( void );
    dsreal GetCurrentAltitud( void );
    dsreal GetCurrentTheta( void );
    dsreal GetCurrentPhi( void );

    void GetXYZ( DrawSpace::Utils::Vector& p_out );


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
