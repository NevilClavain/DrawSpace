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

#ifndef _CHUNK_H_
#define _CHUNK_H_

#include "transformnode.h"
#include "scenegraph.h"
#include "renderer.h"

namespace DrawSpace
{
class Chunk : public Core::TransformNode
{
protected:

    typedef DrawSpace::Core::CallBack<Chunk, void, DrawSpace::Core::RenderingNode*>                                 RenderingNodeDrawCallback;
    typedef DrawSpace::Core::CallBack2<Chunk, void, DrawSpace::Core::LodStep*, DrawSpace::Core::LodStep::Event>     LodCallback;

    DrawSpace::Interface::Renderer*                         m_renderer;
    DrawSpace::Core::Meshe*                                 m_meshe;
    std::map<dsstring, DrawSpace::Core::RenderingNode*>     m_passesnodes;
    std::vector<RenderingNodeDrawCallback*>                 m_callbacks;
    LodCallback*                                            m_lod_callback;
    bool                                                    m_lod_draw;
    DrawSpace::Scenegraph*                                  m_scenegraph; 
    DrawSpace::Core::VSphere*                               m_vsphere;
   
    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );
    void on_lod_event( DrawSpace::Core::LodStep*, DrawSpace::Core::LodStep::Event p_event );

public:
    Chunk( void );
    virtual ~Chunk( void );

    
    virtual void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );
    virtual void OnRegister( DrawSpace::Scenegraph* p_scenegraph );

    virtual DrawSpace::Core::Meshe* GetMeshe( void );
    virtual void SetMeshe( DrawSpace::Core::Meshe* p_meshe );

    virtual void RegisterPassSlot( const dsstring p_passname );
    virtual DrawSpace::Core::RenderingNode* GetNodeFromPass( const dsstring& p_passname );
    
};
}

#endif