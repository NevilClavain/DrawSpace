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

#ifndef _SCENENODEGRAPH_H_
#define _SCENENODEGRAPH_H_

#include "scenenode.h"
#include "pass.h"
#include "callback.h"

namespace DrawSpace
{
namespace Core
{
class SceneNodeGraph
{
public:

    typedef enum
    {
        ACTIVE,

    } CameraEvent;

    typedef DrawSpace::Core::BaseCallback2<void, CameraEvent, BaseSceneNode*>  CameraEventHandler;

protected:

    std::map<dsstring, BaseSceneNode*>          m_nodes;
    std::map<dsstring, Pass*>                   m_passes;
    dsstring                                    m_current_camera;
    std::map<dsstring, Core::BaseSceneNode*>    m_cameras_list;
    Utils::Matrix                               m_view;
    std::vector<CameraEventHandler*>            m_cameraevt_handlers;
   
public:
    SceneNodeGraph( void );
    virtual ~SceneNodeGraph( void );

    void ComputeTransformations( DrawSpace::Utils::TimeManager& p_timemanager );
    std::map<dsstring, BaseSceneNode*>& GetNodesList( void );

    void RegisterPass( Pass* p_pass );

    void RegisterNode( BaseSceneNode* p_node );	
    bool AddNode( BaseSceneNode* p_node );

    Pass* GetPass( const dsstring& p_passname );


    Core::BaseSceneNode* GetCurrentCamera( void );
    bool SetCurrentCamera( const dsstring& p_nodename );

    void GetCurrentCameraView( Utils::Matrix& p_view );
    void GetCurrentCameraTranform( Utils::Matrix& p_mat );
    void GetCurrentCameraProj( Utils::Matrix& p_proj );

    std::map<dsstring, BaseSceneNode*>& GetCamerasList( void );

    void RegisterCameraEvtHandler( CameraEventHandler* p_handler );

    void PointProjection( const DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz );

    void GetCurrentCameraName( dsstring& p_outname );
    
};
}
}

#endif