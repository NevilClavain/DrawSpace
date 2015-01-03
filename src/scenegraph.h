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

#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "pass.h"
#include "callback.h"

namespace DrawSpace
{
class Scenegraph : public Core::TransformQueue
{
public:

    typedef enum
    {
        ACTIVE,

    } CameraEvent;

    typedef DrawSpace::Core::BaseCallback2<void, CameraEvent, Core::TransformNode*>  CameraEventHandler;

protected:
    std::map<dsstring, Pass*>                   m_passes;
    dsstring                                    m_current_camera;
    std::map<dsstring, Core::TransformNode*>    m_cameras_list;
    Utils::Matrix                               m_view;

    std::vector<CameraEventHandler*>            m_cameraevt_handlers;

public:
    Scenegraph( void );
    virtual ~Scenegraph( void );

    void RegisterPass( Pass* p_pass );
    bool RegisterNode( Core::TransformNode* p_node );	
    Pass* GetPass( const dsstring& p_passname );


    Core::TransformNode* GetCurrentCamera( void );
    bool SetCurrentCamera( const dsstring& p_nodename );

    void GetCurrentCameraView( Utils::Matrix& p_view );
    void GetCurrentCameraTranform( Utils::Matrix& p_mat );
    void GetCurrentCameraProj( Utils::Matrix& p_proj );

    void ComputeTransformations( Utils::TimeManager& p_timemanager );

    std::map<dsstring, Core::TransformNode*>& GetCamerasList( void );

    void RegisterCameraEvtHandler( CameraEventHandler* p_handler );
   
    void PointProjection( const DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz );

    void GetCurrentCameraName( dsstring& p_outname );

};
}
#endif
