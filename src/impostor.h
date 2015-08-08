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

#ifndef _IMPOSTOR_H_
#define _IMPOSTOR_H_

#include "renderer.h"
#include "scenenodegraph.h"

namespace DrawSpace
{
class Impostor
{
public:

    typedef struct
    {
        float width_scale;
        float height_scale;
        DrawSpace::Utils::Vector localpos;

        dsreal u1, v1;
        dsreal u2, v2;
        dsreal u3, v3;
        dsreal u4, v4;        

    } DisplayListEntry;

    
    typedef std::vector<DisplayListEntry> DisplayList;

protected:

    typedef DrawSpace::Core::CallBack<Impostor, void, DrawSpace::Core::RenderingNode*> RenderingNodeDrawCallback;

    DrawSpace::Interface::Renderer*                         m_renderer;
    std::map<Pass*, DrawSpace::Core::RenderingNode*>        m_passesnodes;
    std::vector<RenderingNodeDrawCallback*>                 m_callbacks;
    DrawSpace::Core::SceneNodeGraph*                        m_scenenodegraph;
    Utils::Matrix                                           m_globaltransformation;
    DrawSpace::Core::Meshe*                                 m_meshe;

    void Impostor::on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

public:

    Impostor( void );
    virtual ~Impostor( void );

    void Init( const DisplayList& p_list );

    void Update( DrawSpace::Utils::TimeManager& p_timemanager );
    void Update2( DrawSpace::Utils::TimeManager& p_timemanager );
    
    void SetDrawingState( Pass* p_passname, bool p_drawing );

    void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );

    void RegisterPassSlot( Pass* p_pass );
    DrawSpace::Core::RenderingNode* GetNodeFromPass( Pass* p_pass );


    void GetBaseTransform( DrawSpace::Utils::Matrix& p_mat );
    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat );

    void GetPassesNodesList( std::map<Pass*, DrawSpace::Core::RenderingNode*>& p_list );

};
}

#endif