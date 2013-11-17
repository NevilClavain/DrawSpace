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

#include <drawable.h>
#include <scenegraph.h>


class Chunk : public DrawSpace::Interface::Drawable
{
protected:

    typedef DrawSpace::Core::CallBack<Chunk, void, DrawSpace::Core::RenderingNode*> RenderingNodeDrawCallback;

    DrawSpace::Interface::Renderer*                         m_renderer;
    DrawSpace::Core::Meshe*                                 m_meshe;
    std::map<dsstring, DrawSpace::Core::RenderingNode*>     m_passesnodes;
    std::vector<RenderingNodeDrawCallback*>                 m_callbacks;
    DrawSpace::Scenegraph*                                  m_scenegraph;
    

    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

public:
    Chunk( void );
    virtual ~Chunk( void );

    virtual void GetDescr( dsstring& p_descr );
    virtual void DumpMemoryAllocs( void );
    virtual void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );
    virtual void OnRegister( DrawSpace::Scenegraph* p_scenegraph );
    virtual bool LoadAssets( void );
    virtual DrawSpace::Core::Meshe* GetMeshe( const dsstring& p_mesheid );
    virtual void RegisterPassSlot( const dsstring p_passname );
    virtual DrawSpace::Core::RenderingNode* GetNodeFromPass( const dsstring p_passname, const dsstring& p_nodeid );
    virtual void GetNodesIdsList( std::vector<dsstring>& p_ids );
    virtual void ComputeSpecifics( void );
    
};

#endif