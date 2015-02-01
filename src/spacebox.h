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

#ifndef _SPACEBOX_H_
#define _SPACEBOX_H_

#include "transformnode.h"
#include "scenegraph.h"
#include "renderer.h"
#include "scenenodegraph.h"


namespace DrawSpace
{
class Spacebox : public Core::TransformNode
{
public:

    static const int    FrontQuad    = 0;
	static const int    RearQuad     = 1;
	static const int    LeftQuad     = 2;
	static const int    RightQuad    = 3;
	static const int    TopQuad      = 4;
	static const int    BottomQuad   = 5;


    typedef struct 
    {
        dsstring        fx_name;
        long            rendering_order;
        dsstring        textures[6][Core::RenderingNode::NbMaxTextures];

    } PassDescriptor;

    typedef struct
    {
        dsstring                            scene_name;
        std::map<dsstring, PassDescriptor>  passes_slots;

    } Descriptor;


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
    //std::map<dsstring, NodesSet>                            m_passesnodes;
    std::map<Pass*, NodesSet>                               m_passesnodes;
    std::vector<RenderingNodeDrawCallback*>                 m_callbacks;
    DrawSpace::Scenegraph*                                  m_scenegraph;
    DrawSpace::Core::SceneNodeGraph*                        m_scenenodegraph;

    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

public:
    Spacebox( void );
    virtual ~Spacebox( void );

    //void SetDrawingState( const dsstring& p_passname, bool p_drawing );
    void SetDrawingState( Pass* p_pass, bool p_drawing );

    //void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );
    void OnRegister( DrawSpace::Scenegraph* p_scenegraph );
    void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );

    DrawSpace::Core::Meshe* GetMeshe( int p_mesheid );

    //void RegisterPassSlot( const dsstring p_passname );
    void RegisterPassSlot( Pass* p_pass );
    //DrawSpace::Core::RenderingNode* GetNodeFromPass( const dsstring& p_passname, int p_quadid );
    DrawSpace::Core::RenderingNode* GetNodeFromPass( Pass* p_pass, int p_quadid );

    void Update( DrawSpace::Utils::TimeManager& p_timemanager ) {};
    void Update2( DrawSpace::Utils::TimeManager& p_timemanager ) {};

    void GetBaseTransform( DrawSpace::Utils::Matrix& p_mat );
    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat );

    
};
}

#endif
