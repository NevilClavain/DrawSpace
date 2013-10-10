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

#ifndef _CHUNK_NODE_H_
#define _CHUNK_NODE_H_

#include "scenegraph.h"

namespace DrawSpace
{
class ChunkNode : public Core::TransformNode
{
protected:

    typedef Core::CallBack<ChunkNode, void, Core::RenderingNode*>   RenderingNodeDrawCallback;

    Core::Meshe*                                                    m_meshe;
    std::map<dsstring, Core::RenderingNode*>                        m_passesnodes;
    std::vector<RenderingNodeDrawCallback*>                         m_callbacks;

    void on_renderingnode_draw( Core::RenderingNode* p_rendering_node );

public:
    ChunkNode( const dsstring& p_name );
    virtual ~ChunkNode( void );

    Core::Meshe* GetMeshe( void );

    virtual void RegisterPassRenderingNode( const dsstring p_passname, Core::RenderingNode* p_rendering_node  );
    virtual void OnRegister( Scenegraph* p_scenegraph );
    bool LoadAssets( void );

    Core::RenderingNode* GetNodeFromPass( const dsstring& p_passname );
};
}

#endif