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

    typedef DrawSpace::Core::BaseCallback2<void, CameraEvent, BaseSceneNode*>       CameraEventHandler;

    typedef enum
    {
        NODE_ADDED,
        NODE_APP_READY,  // app client custom event

    } NodesEvent;

    typedef DrawSpace::Core::BaseCallback2<void, NodesEvent, BaseSceneNode*>        NodesEventHandler;

    typedef enum
    {
        TRANSFORMATIONS_BEGIN,
        TRANSFORMATIONS_DONE,

    } ScenegraphEvent;

    typedef DrawSpace::Core::BaseCallback2<void, ScenegraphEvent, SceneNodeGraph*>  ScenegraphEventHandler;


protected:

    // liste des nodes "roots", comprenant d'eventuels nodes fils, qui ne sont donc pas stockes ici
    // ce tableau est complete par l'appel a AddNode()

    std::map<dsstring, BaseSceneNode*>          m_nodes;

    // liste exhaustive de tout les nodes, y compris les nodes fils, qui ne sont pas stockes dans m_nodes
    // ce tableau est complete par l'appel a RegisterNode()
    std::vector<BaseSceneNode*>                 m_all_nodes;

    // liste de tout les nodes "camera"
    std::map<dsstring, Core::BaseSceneNode*>    m_cameras_list;


    dsstring                                    m_current_camera;

    //Utils::Matrix                               m_view;

    std::vector<CameraEventHandler*>            m_cameraevt_handlers;
    std::vector<NodesEventHandler*>             m_nodesevt_handlers;
    std::vector<ScenegraphEventHandler*>        m_scenegraphevt_handlers;

   
public:
    SceneNodeGraph( void );
    virtual ~SceneNodeGraph( void );

    void ComputeTransformations( DrawSpace::Utils::TimeManager& p_timemanager );

    // les nodes "root" seulement
    std::map<dsstring, BaseSceneNode*>& GetNodesList( void );

    // tout les nodes
    std::vector<BaseSceneNode*>& GetAllNodesList( void );

    void RegisterNode( BaseSceneNode* p_node );
    void UnregisterNode( BaseSceneNode* p_node );

    bool AddNode( BaseSceneNode* p_node );


    Core::BaseSceneNode* GetCurrentCamera( void );
    bool SetCurrentCamera( const dsstring& p_nodename );

    void GetCurrentCameraView( Utils::Matrix& p_view );
    void GetCurrentCameraTranform( Utils::Matrix& p_mat );
    void GetCurrentCameraProj( Utils::Matrix& p_proj );

    std::map<dsstring, BaseSceneNode*>& GetCamerasList( void );

    void RegisterCameraEvtHandler( CameraEventHandler* p_handler );
    void RegisterNodesEvtHandler( NodesEventHandler* p_handler );
    void RegisterScenegraphEvtHandler( ScenegraphEventHandler* p_handler );

    void PointProjection( const DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz );

    void GetCurrentCameraName( dsstring& p_outname );

    void SendNodeAppReadyEvent( BaseSceneNode* p_node );
    
};
}
}

#endif
