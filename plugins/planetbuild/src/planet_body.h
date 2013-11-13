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

#ifndef _PLANET_BODY_H_
#define _PLANET_BODY_H_

#include <drawable.h>
#include <scenegraph.h>
#include "planet_face.h"

class FaceRenderingNode : public DrawSpace::Core::RenderingNode
{
    typedef DrawSpace::Core::CallBack2<FaceRenderingNode, void, int, Patch*>     PatchInstanciationCallback;
    
protected:

    DrawSpace::Interface::Renderer* m_renderer;
    std::map<dsstring, Patch*>      m_patchesleafs;
    std::map<dsstring, Patch*>      m_patches;
    PatchInstanciationCallback*     m_patchinstanciationcallback;

    void                            on_patchinstanciation( int p_orientation, Patch* p_patch );
    void                            on_patchsplit( int p_orientation, Patch* p_patch );
    
public:
    FaceRenderingNode( DrawSpace::Interface::Renderer* p_renderer );
    virtual ~FaceRenderingNode( void );

    virtual void Draw( const DrawSpace::Utils::Matrix& p_world, DrawSpace::Utils::Matrix& p_view );

    Face::PatchInstanciationHandler* GetPatchInstanciationHandler( void );
};

class Body : public DrawSpace::Interface::Drawable
{	
protected:

	typedef struct
	{
        //DrawSpace::Core::RenderingNode*        nodes[6];

        FaceRenderingNode*        nodes[6];

	} NodesSet;
       
    typedef DrawSpace::Core::CallBack<Body, void, DrawSpace::Core::RenderingNode*> RenderingNodeDrawCallback;
    typedef DrawSpace::Core::CallBack2<Body, void, int, Patch*>                    PatchInstanciationCallback;    

	std::map<dsstring, NodesSet>                m_passesnodes;
	std::vector<RenderingNodeDrawCallback*>     m_callbacks;
    DrawSpace::Scenegraph*                      m_scenegraph;
    DrawSpace::Interface::Renderer*             m_renderer;
    Face*                                       m_faces[6];

    void                                        on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

    //void                                        on_patchinstanciation( int p_orientation, Patch* p_patch );
    
public:

	Body( void );
	virtual ~Body( void );

    virtual void GetDescr( dsstring& p_descr );
    virtual void DumpMemoryAllocs( void );
    virtual void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );
    virtual void OnRegister( DrawSpace::Scenegraph* p_scenegraph );
    virtual bool LoadAssets( void );
    virtual DrawSpace::Core::Meshe* GetMeshe( const dsstring& p_mesheid );
    virtual void RegisterPassSlot( const dsstring p_passname );
    virtual DrawSpace::Core::RenderingNode* GetNodeFromPass( const dsstring p_passname, const dsstring& p_nodeid );
};
#endif