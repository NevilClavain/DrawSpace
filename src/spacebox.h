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

#ifndef _SPACEBOX_H_
#define _SPACEBOX_H_

#include "transformnode.h"
#include "scenegraph.h"
#include "renderer.h"
#include "configurable.h"

#define SPACEBOX_TEXT_KEYWORD           "Spacebox"
#define SPACEBOX_ARC_MAGICNUMBER        0x4043


namespace DrawSpace
{
class Spacebox : public Core::TransformNode, public Core::Configurable
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
    std::map<dsstring, NodesSet>                            m_passesnodes;
    std::vector<RenderingNodeDrawCallback*>                 m_callbacks;
    DrawSpace::Scenegraph*                                  m_scenegraph;

    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

    bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:
    Spacebox( void );
    virtual ~Spacebox( void );

    void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );
    void OnRegister( DrawSpace::Scenegraph* p_scenegraph );

    DrawSpace::Core::Meshe* GetMeshe( int p_mesheid );

    void RegisterPassSlot( const dsstring p_passname );
    DrawSpace::Core::RenderingNode* GetNodeFromPass( const dsstring& p_passname, int p_quadid );

    void Serialize( Utils::Archive& p_archive  );
    bool Unserialize( Utils::Archive& p_archive );

    void DumpProperties( dsstring& p_text );
    bool ParseProperties( const dsstring& p_text );

    void ApplyProperties( void );

    
};
}

#endif