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

#ifndef _CHUNK_H_
#define _CHUNK_H_

#include "renderer.h"
#include "scenenodegraph.h"
#include "impostorsdisplaylist.h"

namespace DrawSpace
{
class Chunk
{
protected:

    typedef DrawSpace::Core::CallBack<Chunk, void, DrawSpace::Core::RenderingNode*>                                 RenderingNodeDrawCallback;
    

    DrawSpace::Interface::Renderer*                         m_renderer;
    DrawSpace::Core::Meshe*                                 m_meshe;
    DrawSpace::ImpostorsDisplayList                         m_idl;

    std::map<Pass*, DrawSpace::Core::RenderingNode*>        m_passesnodes;

    std::vector<RenderingNodeDrawCallback*>                 m_callbacks;

    DrawSpace::Core::SceneNodeGraph*                        m_scenenodegraph;
    bool                                                    m_ignore_camera;
    Utils::Matrix                                           m_globaltransformation;

   
    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

public:
    Chunk( void );
    virtual ~Chunk( void );

    void SetImpostorsDisplayList( const DrawSpace::ImpostorsDisplayList& p_idl );
    void ImpostorsInit( /*const ImpostorsDisplayList& p_list*/ void );
    void ImpostorsUpdate( void );

    void Update( DrawSpace::Utils::TimeManager& p_timemanager ) {};
    void Update2( DrawSpace::Utils::TimeManager& p_timemanager );
   
    void SetDrawingState( Pass* p_passname, bool p_drawing );

    void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );

    DrawSpace::Core::Meshe* GetMeshe( void );
    void SetMeshe( DrawSpace::Core::Meshe* p_meshe );

    void RegisterPassSlot( Pass* p_pass );

    DrawSpace::Core::RenderingNode* GetNodeFromPass( Pass* p_pass );

    void GetBaseTransform( DrawSpace::Utils::Matrix& p_mat );
    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat );

    void IgnoreCamera( bool p_ignore );

    void GetPassesNodesList( std::map<Pass*, DrawSpace::Core::RenderingNode*>& p_list );
   
};
}

#endif
