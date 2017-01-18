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

#ifndef _SPACEBOX_H_
#define _SPACEBOX_H_

#include "renderer.h"
#include "scenenodegraph.h"


namespace DrawSpace
{
class Spacebox
{
public:

    static const int    FrontQuad    = 0;
	static const int    RearQuad     = 1;
	static const int    LeftQuad     = 2;
	static const int    RightQuad    = 3;
	static const int    TopQuad      = 4;
	static const int    BottomQuad   = 5;

protected:

    typedef DrawSpace::Core::CallBack<Spacebox, void, DrawSpace::Core::RenderingNode*>   RenderingNodeDrawCallback;

	typedef struct
	{
        DrawSpace::Core::RenderingNode*        nodes[6];

	} NodesSet;

	typedef struct
	{
        dsstring                texturenames[6];

	} TexturesNameSet;


    DrawSpace::Interface::Renderer*                         m_renderer;
    DrawSpace::Core::Meshe*                                 m_meshes[6];
    std::map<Pass*, NodesSet>                               m_passesnodes;
    std::vector<RenderingNodeDrawCallback*>                 m_callbacks;
    DrawSpace::Core::SceneNodeGraph*                        m_scenenodegraph;
    bool                                                    m_enable_translations;
    bool                                                    m_ignore_camera;
    Utils::Matrix                                           m_globaltransformation;

    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

public:
    Spacebox( void );
    virtual ~Spacebox( void );

    void SetDrawingState( Pass* p_pass, bool p_drawing );
    void SetQuadDrawingState( Pass* p_pass, int p_quad, bool p_drawing );

    void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );
    void OnUnregister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );

    DrawSpace::Core::Meshe* GetMeshe( int p_mesheid );

    void RegisterPassSlot( Pass* p_pass );
    DrawSpace::Core::RenderingNode* GetNodeFromPass( Pass* p_pass, int p_quadid );

    void Update( DrawSpace::Utils::TimeManager& p_timemanager ) {};
    void Update2( DrawSpace::Utils::TimeManager& p_timemanager ) {};

    void GetBaseTransform( DrawSpace::Utils::Matrix& p_mat );
    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat );

    void EnableTranslations( bool p_enable );
    void IgnoreCamera( bool p_ignore );

    void GetPassesNodesList( std::map<Pass*, NodesSet>& p_list );
};
}

#endif
