/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#ifndef _CLOUDS_H_
#define _CLOUDS_H_

#include "chunk.h"
#include "callback.h"
#include "procedural_blocs.h"
#include "task.h"
#include "runner.h"
#include "camerapoint.h"

namespace DrawSpace
{
class Clouds : public Chunk
{
public:
    typedef Core::CallBack<Clouds, void, Procedural::Atomic*>                                        ProceduralCb;

protected:

    typedef Core::CallBack2<Clouds, void, Core::SceneNodeGraph::CameraEvent, Core::BaseSceneNode*>   CameraEventCb;

public:

    

    typedef struct
    {
        DrawSpace::ImpostorsDisplayList         impostors;

        DrawSpace::Utils::Vector                position;
        dsreal                                  distToView; // compute result for z qsort
        
    } CloudUnitDescriptor;

protected:

    void on_camera_event( Core::SceneNodeGraph::CameraEvent p_event, Core::BaseSceneNode* p_node );
    void on_procedural( Procedural::Atomic* p_atom );

    void execsortz( const DrawSpace::Utils::Matrix& p_impostor_mat, const DrawSpace::Utils::Matrix& p_cam_mat );
    static bool nodes_comp( CloudUnitDescriptor* p_n1, CloudUnitDescriptor* p_n2 );
    void impostors_init( void );
   

    ProceduralCb*                                                       m_proceduralcb;
    CameraEventCb*                                                      m_cameracb;

    bool                                                                m_clouds_sort_request;

    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_current_camera;

    bool                                                                m_previous_camera_pos_avail;
    DrawSpace::Utils::Vector                                            m_previous_camera_pos;

    std::vector<CloudUnitDescriptor*>                                   m_clouds;
    CloudUnitDescriptor*                                                m_new_cloud;

    DrawSpace::Core::BaseSceneNode*                                     m_owner;

    dsreal                                                              m_sorting_distance;
    bool                                                                m_details;
    
    bool                                                                m_running;

    int                                                                 m_nbmax_clouds_impostors;

public:

    Clouds( void );
    virtual ~Clouds( void );

    void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );
    ProceduralCb* GetProceduralCallback( void );
    void Update2( DrawSpace::Utils::TimeManager& p_timemanager );

    void SetSortingDistance( dsreal p_distance );
    dsreal GetSortingDistance( void );
    void EnableDetails( bool p_details );

    void ImpostorsInit( void );
    void CloudsReset( void );
    void CloudsUpdateRequest( void );
};
}

#endif
